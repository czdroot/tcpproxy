# tcp proxy
- 这个工具的原始目的是一些公司只有 windows 的代理认证，没有linux代理认证，让linux机器能通过windows机器访问互联网的工具。这时windows机器作为一个透明的tcp转发机器，把本地IP收到的请求转发到 DEF_PROXY_IP, 又把 DEF_PROXY_IP 收到的响应转发给 请求者。 

- 另外一个目的是扫描网络中的linux机器（通过判断机器的22号端口是否打开）。

- 不带任何参数时，默认作为TCP 代理运行，把本地第一个IP地址的指定端口收到的报文转发刚才另外一个远程IP地址(DEF_PROXY_IP)的相同端口。当远程IP返回报文时，转发给原始端口，实现透明代理。


- 现在它只转发 80, 443, 9418 端口报文。如果想扩展，自己修改下面变量：
int     g_proxy_ports[PROXY_PORT_CNT]   = {80,  //  http
在里面添加新端口号，同时修改  PROXY_PORT_CNT的值。

- 如果电脑配置多个IP，那用 "-l 本地IP地址"指定监听的IP。

- 如果远程地址与DEF_PROXY_IP不同，那使用 " -p 代理IP" 指定远程IP。

- 如果想扫描LINUX机器，使用 "-s ipaddr/mask[:port=22]" 来指定扫描的IP地址段，例如： -s 192.168.1.1/24 就是扫描192.168.1.0-192.168.1.255所有的22号端口。



