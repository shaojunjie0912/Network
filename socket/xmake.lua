target("hello_server")
    set_kind("binary")
    add_files("hello_server.cpp")

target("hello_client")
    set_kind("binary")
    add_files("hello_client.cpp")