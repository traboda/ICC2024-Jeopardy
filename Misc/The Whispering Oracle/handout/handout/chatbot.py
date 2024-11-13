import re
import random
import sys


def get_response(user_message):
    user_message = user_message.lower()  # normalize to lowercase

    # Define possible responses
    if re.search(r'\b(hello|hi|hey|greetings)\b', user_message):
        return "Hello! How can I help you today?"

    elif re.search(r'\b(how are you|how’s it going|what’s up)\b', user_message):
        return "I'm just a bot, but thanks for asking! How are you?"

    elif re.search(r'\b(help|support|assist|problem)\b', user_message):
        return "I'm here to help! Could you describe your issue?"

    elif re.search(r'\b(thanks|thank you|appreciate)\b', user_message):
        return "You're welcome! Let me know if you need anything else."

    elif re.search(r'\b(bye|goodbye|see you|later)\b', user_message):
        return "Goodbye! Have a great day!"

    else:
        # Randomized responses for unrecognized input
        unknown_responses = [
            "I'm not sure I understand. Could you rephrase that?",
            "Hmm, that’s new to me! Could you explain it differently?",
            "I don’t quite get that. Can you give me more details?",
            "I’m still learning, so I may not understand everything yet!",
            "That sounds interesting! Tell me more about it.",
            "I'm sorry, I'm not sure what you mean. Let's try again!"
        ]
        return random.choice(unknown_responses)


if __name__ == "__main__":
    # Check if input is provided as a command-line argument
    if len(sys.argv) < 2:
        print("Please provide a message for the chatbot as a command-line argument.")
    else:
        user_input = " ".join(sys.argv[1:])  # Combine all command-line arguments into one message
        response = get_response(user_input)
        print("Chatbot:", response)

