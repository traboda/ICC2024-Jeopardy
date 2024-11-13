#include <iostream>
#include <cstring>
#include <string>
#include <stack>
#include <memory>
#include <unordered_map>
#include <sys/mman.h>

#define PAGE_SZ        (0x1000)
#define PAGE_ALIGN(x)  (x%PAGE_SZ ? (x&~0xfffL)+PAGE_SZ : x)
#define FETCH_SZ       (7L)
#define EXEC_SZ        (6L)

typedef long (jitFunc)(long *, long, long);

class Node {
    public:
        bool isOperator;
        long value;
        char op;
        std::unique_ptr<Node> left, right;


        Node(char op) : isOperator(true), op(op), value(0), left(nullptr), right(nullptr) {}
        Node(double value) : isOperator(false), value(value), op(0), left(nullptr), right(nullptr) {}
};


class Parser {
    private:
        bool isOperator(char token);
        int  precedence(char op);
        void processOperator(std::stack<char>& operators, std::stack<std::unique_ptr<Node>>& operands);

    public:
        std::unique_ptr<Node> parseExpression(const std::string& expression);
};


class Evaluator {
    private:
        char *jitCode, *jitCodeBase;
        long *jitConsts, *jitConstsBase;
        long jitConstCount, jitOpCount;
        long jitConstsSize, jitCodeSize;
        long jitPrevConst;

        std::unordered_map<long, short> constMap;

        void *createJitRegion(long size);
        void updateJitPerm(void);
        void jitEmitCode(const std::unique_ptr<Node>& root);
        void jitCountNodes(const std::unique_ptr<Node> & root);

    public:
        long evaluate(const std::unique_ptr<Node>& root);
        long jitEvaluate(const std::unique_ptr<Node>& root);
        ~Evaluator(void);
};