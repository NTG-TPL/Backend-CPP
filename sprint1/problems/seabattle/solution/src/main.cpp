#ifdef WIN32
#include <sdkddkver.h>
#endif

#include "seabattle.h"

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace net = boost::asio;
using net::ip::tcp;
using namespace std::literals;

void PrintFieldPair(const SeabattleField& left, const SeabattleField& right) {
    auto left_pad = "  "s;
    auto delimeter = "    "s;
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
    for (size_t i = 0; i < SeabattleField::field_size; ++i) {
        std::cout << left_pad;
        left.PrintLine(std::cout, i);
        std::cout << delimeter;
        right.PrintLine(std::cout, i);
        std::cout << std::endl;
    }
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
}

template <size_t sz>
static std::optional<std::string> ReadExact(tcp::socket& socket) {
    boost::array<char, sz> buf{};
    boost::system::error_code ec;

    net::read(socket, net::buffer(buf), net::transfer_exactly(sz), ec);

    if (ec) {
        return std::nullopt;
    }

    return {{buf.data(), sz}};
}

static bool WriteExact(tcp::socket& socket, std::string_view data) {
    boost::system::error_code ec;

    net::write(socket, net::buffer(data), net::transfer_exactly(data.size()), ec);

    return !ec;
}

class SeabattleAgent {
public:
    explicit SeabattleAgent(const SeabattleField& field)
        : my_field_(field) {
    }

    void StartGame(tcp::socket& socket, bool my_initiative) {
        PrintFields();
        while (!IsGameEnded()) {
            if(!my_initiative){
                std::cout << "Waiting for turn... " << std::endl;
                auto client_data = ReadExact<2>(socket);
                std::cout << "Shot to " << client_data->data() << std::endl;
                auto move = ParseMove(client_data->data());
                auto shoot_result = my_field_.Shoot(move->second, move->first);

                if(shoot_result == SeabattleField::ShotResult::MISS){
                    my_initiative = !my_initiative;
                }

                WriteExact(socket, std::string(1, static_cast<char>(static_cast<int>(shoot_result))));
            }else {
                auto shoot = GetCorrectMove();
                WriteExact(socket, shoot);

                auto client_data = ReadExact<1>(socket);
                auto move = ParseMove(shoot);
                auto shoot_result = static_cast<SeabattleField::ShotResult>(client_data->data()[0]);

                switch (shoot_result) {
                    case SeabattleField::ShotResult::MISS:
                        other_field_.MarkMiss(move->second, move->first);
                        std::cout << "MISS!" << std::endl;
                        my_initiative = !my_initiative;
                        break;
                    case SeabattleField::ShotResult::HIT:
                        other_field_.MarkHit(move->second, move->first);
                        std::cout << "HIT!" << std::endl;
                        break;
                    case SeabattleField::ShotResult::KILL:
                        other_field_.MarkKill(move->second, move->first);
                        std::cout << "KILL!" << std::endl;
                        break;
                    }
            }
            PrintFields();
        }
        if(other_field_.IsLoser()){
            std::cout << "You Won !!!" << std::endl;
        }else{
            std::cout << "You've Lost" << std::endl;
        }
    }

private:
    static std::optional<std::pair<int, int>> ParseMove(const std::string_view& sv) {
        if (sv.size() != 2) return std::nullopt;

        int p1 = sv[0] - 'A', p2 = sv[1] - '1';

        if (p1 < 0 || p1 > 8) return std::nullopt;
        if (p2 < 0 || p2 > 8) return std::nullopt;

        return {{p1, p2}};
    }

    static std::string MoveToString(std::pair<int, int> move) {
        char buff[] = {static_cast<char>(move.first + 'A'), static_cast<char>(move.second + '1')};
        return {buff, 2};
    }

    void PrintFields() const {
        PrintFieldPair(my_field_, other_field_);
    }

    [[nodiscard]] bool IsGameEnded() const {
        return my_field_.IsLoser() || other_field_.IsLoser();
    }

private:
    SeabattleField my_field_;
    SeabattleField other_field_;

    static std::string GetCorrectMove(){
        std::string shoot;
        std::cout << "Your turn: ";
        std::getline(std::cin, shoot);
        auto move = ParseMove(shoot);
        while (move == std::nullopt){
            std::cout << "Something went wrong... try again: ";
            std::getline(std::cin, shoot);
            move = ParseMove(shoot);
        }
        return shoot;
    }
};

void StartServer(const SeabattleField& field, unsigned short port) {
    SeabattleAgent agent(field);

    net::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
    std::cout << "Waiting for connection..."sv << std::endl;

    boost::system::error_code ec;
    tcp::socket socket{io_context};
    acceptor.accept(socket, ec);

    if (ec) {
        throw std::runtime_error("Can't accept connection");
    }

    agent.StartGame(socket, false);
    }

void StartClient(const SeabattleField& field, const std::string& ip_str, unsigned short port) {
    SeabattleAgent agent(field);

    boost::system::error_code ec;
    auto endpoint = tcp::endpoint(net::ip::make_address(ip_str, ec), port);

    if (ec) {
        throw std::runtime_error("Wrong IP format");
    }

    net::io_context io_context;
    tcp::socket socket{io_context};
    socket.connect(endpoint, ec);

    if (ec) {
        throw std::runtime_error("Can't connect to server");
    }

    agent.StartGame(socket, true);
}

int main(int argc, const char** argv) {
    if (argc != 3 && argc != 4) {
        std::cout << "Usage: program <seed> [<ip>] <port>" << std::endl;
        return 1;
    }

    std::mt19937 engine(std::stoi(argv[1]));
    SeabattleField fieldL = SeabattleField::GetRandomField(engine);
    try {
        if (argc == 3) {
            StartServer(fieldL, std::stoi(argv[2]));
        } else {
            StartClient(fieldL, argv[2], std::stoi(argv[3]));
        }
    }catch(std::runtime_error& er){
        std::cerr << er.what() << std::endl;
    }
}
