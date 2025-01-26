#include <arpa/inet.h>
#include <fmt/color.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>

void ErrorHandling(const std::string& message) {
    fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "{}\n", message);
    exit(1);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        ErrorHandling("Usage: ./hello_server <port>\n");
    }

    const uint16_t port = atoi(argv[1]);

    // 1. 创建服务器监听套接字
    int listen_socket{socket(AF_INET, SOCK_STREAM, 0)};
    if (listen_socket == -1) {
        ErrorHandling("socket() error");
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_port = htons(port);  // 主机字节序->网络字节序
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // 2. 绑定监听套接字
    int ret = bind(listen_socket, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr));
    if (ret == -1) {
        ErrorHandling("bind() error");
    }

    // 3. 开始监听
    ret = listen(listen_socket, 5);
    if (ret == -1) {
        ErrorHandling("listen() error");
    }
    sockaddr_in clnt_addr;
    uint32_t clnt_addr_size{sizeof(clnt_addr)};
    // 4. 接受客户端请求
    // 5. 返回通信套接字
    int communicate_socket = accept(listen_socket, reinterpret_cast<sockaddr*>(&clnt_addr), &clnt_addr_size);
    if (communicate_socket == -1) {
        ErrorHandling("accept() error");
    }
    // 打印客户端的ip和端口(网络字节序 -> 主机字节序)
    char ip[50] = {0};
    fmt::print("Connected client: {}, port: {}\n", inet_ntop(AF_INET, &clnt_addr.sin_addr.s_addr, ip, sizeof(ip)),
               ntohs(clnt_addr.sin_port));

    while (true) {
        char buf[1024]{};
        int len = recv(communicate_socket, buf, sizeof(buf), 0);
        if (len > 0) {
            fmt::println("客户端say: {}", buf);
            send(communicate_socket, buf, len, 0);
        } else if (len == 0) {
            fmt::println("客户端断开连接");
            break;
        } else {
            ErrorHandling("read error");
        }
    }
    close(listen_socket);
    close(communicate_socket);
    return 0;
}
