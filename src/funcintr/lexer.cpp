#include <memory>
#include <sstream>
#include <string>
#include <s3dg/funcintr/lexer.h>
#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <spdlog/spdlog.h>


namespace s3dg {
    namespace lexer {

        bool Token::operator==(const Token& other) {
            return type == other.type && buf == other.buf;
        }

        bool Token::operator==(const TokenType& other) {
            return type == other;
        }

        char get_nonblank(std::stringstream& ss) {
            char buf;
            buf = ss.get();

            while (isblank(buf)) {
                buf = ss.get();
            }

            return buf;
        }

        std::unique_ptr<Token> get_tok(std::stringstream& ss) {
            auto tok = std::make_unique<Token>();
            std::stringstream stream_tok_buf;
            char tok_buf = get_nonblank(ss);

            if (!ss || tok_buf == '\n') {
                tok->type = tok_eos;
                return tok;
            }

            if (isalpha(tok_buf)) { // [a-zA-Z][a-zA-Z0-9]+
                while (isalnum(tok_buf)) {
                    stream_tok_buf << tok_buf;
                    tok_buf = ss.get();
                }

                ss.unget(); // isn't alnum
                auto tmp_buf = stream_tok_buf.str();

                if (tmp_buf == "define") {
                    tok->type = tok_define;
                    return tok;
                }

                tok->buf = std::move(tmp_buf);
                tok->type = tok_ident;
                return tok;
            }

            if (isdigit(tok_buf)) { // [0-9]+\.[0-9]+
                // TODO: add error for multi .
                while (isdigit(tok_buf) || tok_buf == '.') {
                    stream_tok_buf << tok_buf;
                    tok_buf = ss.get();
                }

                ss.unget(); // isn't number
                tok->type = tok_number;
                tok->buf = stream_tok_buf.str();
                return tok;
            }

            if (tok_buf == '(') {
                tok->type = tok_obrac;
                return tok;
            }

            if (tok_buf == ')') {
                tok->type = tok_cbrac;
                return tok;
            }

            tok->type = tok_any;
            tok->buf += tok_buf;
            return tok;
        }

        bool is_next_tok(LexerQueue lexer_queue, const Token& other) {
            return is_next_tok(lexer_queue, other.buf) && is_next_tok(lexer_queue, other.type);
        }

        bool is_next_tok(LexerQueue lexer_queue, const TokenType& other) {
            if (lexer_queue) {
                return lexer_queue->front()->type == other;
            }

            return false;
        }

        bool is_next_tok(LexerQueue lexer_queue, const std::string& other) {
            return lexer_queue->front()->buf == other;
        }

        LexerQueue lexer(std::stringstream& ss) {
            auto tok_stack = std::make_shared<std::deque<std::unique_ptr<Token>>>();

            do {
                auto tok = get_tok(ss);
                tok_stack->push_back(std::move(tok));
            } while (ss.peek() != EOF && ss.peek() != 0);

            if (tok_stack->back()->type != tok_eos) {
                auto tok = std::make_unique<Token>();
                tok->type = tok_eos;
                tok_stack->push_back(std::move(tok));
            }

            return tok_stack;
        }

        void debug_lexer_queue(LexerQueue lexer_queue) {
            std::map<TokenType, std::string> pretty_names = {
                {tok_any, "ANY"},
                {tok_ident, "IDENT"},
                {tok_define, "DEFINE"},
                {tok_number, "NUMBER"},
                {tok_eos, "EOS"},
                {tok_obrac, "OPEN BRAC"},
                {tok_cbrac, "CLOSE BRAC"},
            };
            std::queue<std::unique_ptr<Token>> buffer;
            size_t i = 1;
            spdlog::debug("Queue Size: {}", lexer_queue->size());

            while (!lexer_queue->empty()) {
                auto tok = std::move(lexer_queue->front());
                lexer_queue->pop_front();
                spdlog::debug("\tPos: {}", i);
                spdlog::debug("\tType: {}", pretty_names[tok->type]);
                spdlog::debug("\tBuffer: {}", tok->buf);
                ++i;
                buffer.push(std::move(tok));
            }

            while (!buffer.empty()) {
                auto tok = std::move(buffer.front());
                buffer.pop();
                lexer_queue->push_back(std::move(tok));
            }
        }

    }
}
