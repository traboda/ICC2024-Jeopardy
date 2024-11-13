#include "calc.hxx"


bool Parser::isOperator(char token) {
    return token == '+' || token == '-' || token == '*' || token == '/' || token == '^';
}

int Parser::precedence(char op) {
    switch (op) {
        case '-': return 1;
        case '+': return 2;
        case '*': return 3;
        case '/': return 4;
        case '^': return 5;
        default: return 0;
    }
}

void Parser::processOperator(std::stack<char>& operators, std::stack<std::unique_ptr<Node>>& operands) {
    char op = operators.top(); operators.pop();

    if(operands.size() < 2) {
        throw std::runtime_error("Malformed expression");
    }

    auto opNode = std::make_unique<Node>(op);
    opNode->left = std::move(operands.top()); operands.pop();
    opNode->right = std::move(operands.top()); operands.pop();

    operands.push(std::move(opNode));
}

// void Parser::dumpExpression(const std::unique_ptr<Node>& root) {

//     if (!root) return;

//     if (!root->isOperator) {
//         std::cout << root->value;
//         return;
//     }

//     std::cout << '(';
//     dumpExpression(root->left);

//     std::cout << root->op;

//     dumpExpression(root->right);
//     std::cout << ')';

// }

std::unique_ptr<Node> Parser::parseExpression(const std::string& expression) {
    std::stack<std::unique_ptr<Node>> operands;
    std::stack<char> operators;

    for (size_t i = 0; i < expression.size(); i++) {
        char token = expression[i];

        if (isspace(token)) continue;

        if (isdigit(token)) {
            std::string numStr;
            while (i < expression.size() && isdigit(expression[i])) {
                numStr += expression[i++];
            }
            --i;

            operands.push(std::make_unique<Node>(std::stod(numStr)));
        }

        else if (token == '(') {
            operators.push(token);
        }

        else if (token == ')') {
            while (!operators.empty() && operators.top() != '(') {
                processOperator(operators, operands);
            }

            if(operators.top() != '(') throw std::runtime_error("Imbalanced parenthesis");
            operators.pop();
        }

        else if (isOperator(token)) {
            while (!operators.empty() && precedence(operators.top()) >= precedence(token)) {
                processOperator(operators, operands);
            }
            operators.push(token);
        }

        else {
            std::string errStr;
            errStr = "Invalid character during parsing: ";
            errStr.append(&token, 1);
            throw std::runtime_error(errStr);
        }
    }

    while (!operators.empty()) {
        processOperator(operators, operands);
    }

    return std::move(operands.top());
}


void *Evaluator::createJitRegion(long size) {
    void *region = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);
    if(region == MAP_FAILED) throw std::runtime_error("Unable to allocate JIT memory");
    return region;
}

void Evaluator::updateJitPerm(void) {

    if(mprotect(jitCodeBase, jitCodeSize, PROT_READ | PROT_EXEC) ||
        mprotect(jitConstsBase, jitConstsSize, PROT_READ))
        throw std::runtime_error("Unable to set JIT memory permissions");
    return;

}

long Evaluator::evaluate(const std::unique_ptr<Node>& root) {
    if (!root) return 0;

    if (!root->isOperator) {
        return root->value;
    }

    long leftVal = evaluate(root->left);
    long rightVal = evaluate(root->right);

    switch (root->op) {
        case '+': return leftVal + rightVal;
        case '-': return leftVal - rightVal;
        case '*': return leftVal * rightVal;
        case '^': return leftVal ^ rightVal;
        case '/': 
            if (rightVal == 0) throw std::runtime_error("Division by zero");
            return leftVal / rightVal;
        default: throw std::runtime_error("Invalid operator");
    }
}

void Evaluator::jitCountNodes(const std::unique_ptr<Node> & root) {

    if(!root) return;

    if (!root->isOperator) {
        jitConstCount++;
        return;
    }
    jitCountNodes(root->left);
    jitCountNodes(root->right);
    jitOpCount++;

    return;
}

void Evaluator::jitEmitCode(const std::unique_ptr<Node>& root) {

    if(!root) return;

    if (!root->isOperator) {

        if(constMap.find(root->value) == constMap.end()) {
            // std::cout << "Populating: " << root->value << std::endl;
            *jitConsts = root->value;
            constMap[root->value] = jitConsts-jitConstsBase;
            jitConsts++;
        }

        *jitCode++ = '\x66';
        *jitCode++ = '\xba';
        *(short *)jitCode = constMap[root->value];
        jitCode += sizeof(short);

        *jitCode++ = '\xff';
        *jitCode++ = '\x34';
        *jitCode++ = '\xd7';

        if(jitPrevConst == root->value) {
            *(int *)(jitCode-7) = 0x90909090;
        }
        else {
            jitPrevConst = root->value;
            // std::cout << "rdx <== " << constMap[root->value] << std::endl;
        }

        // std::cout << "push [rdi+rdx*8]: " << root->value << std::endl;

        return;
    }

    jitEmitCode(root->left);
    jitEmitCode(root->right); 

    *jitCode++ = '\x5e';
    *jitCode++ = '\x58';
    *jitCode++ = '\x48';

    switch (root->op) {

        case '+':
            *jitCode++ = '\x01';
            *jitCode++ = '\xf0';
            // std::cout << "add rax, rsi" << std::endl;
            break;

        case '-':
            *jitCode++ = '\x29';
            *jitCode++ = '\xf0';
            // std::cout << "sub rax, rsi" << std::endl;
            break;

        case '*':
            *jitCode++ = '\xf7';
            *jitCode++ = '\xee';
            // std::cout << "imul rsi" << std::endl;
            break;

        case '^':
            *jitCode++ = '\x31';
            *jitCode++ = '\xf0';
            // std::cout << "xor rax, rsi" << std::endl;
            break;

        case '/': 
            jitCode--;
            *jitCode++ = '\x40';
            *jitCode++ = '\xf6';
            *jitCode++ = '\xf6';
            // std::cout << "div sil" << std::endl;
            break;

        default: 
            throw std::runtime_error("Invalid operator");
    }
    *jitCode++ = '\x50';    

    return;
}

long Evaluator::jitEvaluate(const std::unique_ptr<Node>& root) {

    if (!root) return 0;

    jitConstCount = jitOpCount = 0;
    jitCountNodes(root);
    jitConstsSize = PAGE_ALIGN((jitConstCount*sizeof(long)));
    jitCodeSize = PAGE_ALIGN(((jitConstCount*FETCH_SZ)+(jitOpCount*EXEC_SZ)));

    // std::cout << "JitOpCount:    " << jitOpCount << std::endl;
    // std::cout << "JitConstCount: " << jitConstCount << std::endl;

    // std::cout << "JitConstsSize: " << jitConstsSize << std::endl;
    // std::cout << "JitCodeSize  : " << jitCodeSize << std::endl;

    jitCodeBase = (char *)createJitRegion(jitConstsSize + jitCodeSize);
    jitConstsBase = jitConsts = (long *)(jitCodeBase);

    jitCodeBase += jitConstsSize;
    jitCode = jitCodeBase;

    std::memset(jitCodeBase, 0x90, jitCodeSize);
    *(jitCode+jitCodeSize-2) = '\x58';
    *(jitCode+jitCodeSize-1) = '\xc3';

    jitEmitCode(root);
    updateJitPerm();
    
    // getchar();
    return ((jitFunc *)jitCodeBase)(jitConstsBase, 0, 0);

}

Evaluator::~Evaluator(void) {
    if(jitConstsBase && munmap(jitConstsBase, jitConstsSize+jitCodeSize)) {
        std::cout << "Unable to unmap JIT memory" << std::endl;
        exit(1);
    }
}


int main() {
    Parser parser;
    Evaluator *evaluator;
    bool jit = false;

    std::string expression;

    std::cout.rdbuf()->pubsetbuf(0, 0);
    std::cin.rdbuf()->pubsetbuf(0, 0);
    
    std::cout << "Enter expression (type 'jit' to toggle jit):\n";
    while (true) {

        long result = 0, jit_result = 0;

        std::cout << "> ";
        std::getline(std::cin, expression);

        if (expression.size() < 1 || expression.size() > 0xffff) {
            break;
        }

        else if (expression == "jit") {
            jit = !jit;
            continue;
        }

        try {
            auto root = parser.parseExpression(expression);

            evaluator = new Evaluator();
            result = evaluator->evaluate(root);

            if(jit) {
                jit_result = evaluator->jitEvaluate(root);
                if(jit_result != result) throw std::runtime_error("JIT error!");
            }
            
            std::cout << "Result: " << result << std::endl;

            delete evaluator;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}
