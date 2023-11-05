#include "audio.h"
#include <boost/asio.hpp>
#include <iostream>

namespace net = boost::asio;
namespace sys = boost::system;
using net::ip::udp;

using namespace std::literals;

[[noreturn]] void StartServer(uint16_t port){
    Player player(ma_format_u8, 1);
    static const size_t max_buffer_size = 65000;

    net::io_context io_context;
    udp::socket socket(io_context, udp::endpoint(udp::v4(), port));

    // Запускаем сервер в цикле, чтобы можно было работать со многими клиентами
    for (;;) {
        // Создаём буфер достаточного размера, чтобы вместить датаграмму.
        std::array<char, max_buffer_size> recv_buf{};
        udp::endpoint remote_endpoint;

        // Получаем не только данные, но и endpoint клиента
        auto buff_size = socket.receive_from(net::buffer(recv_buf), remote_endpoint);
        auto buff_frames = buff_size / player.GetFrameSize();

        // Воспроизводим сообщение
        player.PlayBuffer(recv_buf.data(), buff_frames, 1.5s);
    }
}

void StartClient(uint16_t port){
    Recorder recorder(ma_format_u8, 1);
    net::io_context io_context;
    udp::socket socket(io_context, udp::v4());

    std::string str;

    std::cout << "Press Enter to record message..." << std::endl;
    std::getline(std::cin, str);

    sys::error_code ec;
    auto endpoint = udp::endpoint(net::ip::make_address(str, ec), port);

    auto rec_result = recorder.Record(65000, 1.5s);
    std::cout << "Recording done" << std::endl;

    auto buffRec = rec_result.frames * recorder.GetFrameSize();
    std::cout << "Frames " << rec_result.frames << " Size: " << buffRec << std::endl;
    socket.send_to(net::buffer(rec_result.data, buffRec), endpoint);

}

int main(int argc, char** argv) {

    if (argc != 3) {
        std::cout << "Usage: "sv << argv[0] << " <client/server> <port>"sv << std::endl;
        return 1;
    }
    std::string mode;
    if (argv[1] == "server"s) {
        mode = "server"s;
    } else if (argv[1] == "client"s) {
        mode = "client"s;
    } else {
        std::cout << "Usage: "sv << argv[1] << " <client/server> <port>"sv << std::endl;
        return 1;
    }

    uint16_t port = std::stoi(argv[2]);
    try {
        std::cout << "Start " << mode << std::endl;
        if (mode == "client"){
            StartClient(port);
        } else if (mode == "server"){
            StartServer(port);
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}