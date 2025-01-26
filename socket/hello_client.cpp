#include <arpa/inet.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>

void ErrorHandling(const std::string& message) {
    fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "{}\n", message);
    exit(1);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        ErrorHandling("Usage: ./hello_client <ip> <port>\n");
    }
    char const* ip = argv[1];
    uint16_t const port = atoi(argv[2]);

    // 创建客户端连接套接字
    int sock{socket(PF_INET, SOCK_STREAM, 0)};

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &serv_addr.sin_addr.s_addr);

    int ret = connect(sock, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr));
    if (ret == -1) {
        ErrorHandling("connect error");
    }
    int num{0};
    while (true) {
        char buf[1024]{};
        sprintf(buf, "你好, 服务器...%d\n", num++);
        write(sock, buf, strlen(buf) + 1);
        memset(buf, 0, sizeof(buf));
        int len = read(sock, buf, sizeof(buf));
        if (len > 0) {
            printf("服务器say: %s\n", buf);
        } else if (len == 0) {
            printf("服务器断开了连接...\n");
            break;
        } else {
            perror("read");
            break;
        }

        sleep(1);
    }
    close(sock);
    return 0;
}
