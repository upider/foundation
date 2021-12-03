# select and channel

## 用途

1. I/O 多路复用
2. 条件语句（类似golang）

## 类

包装操作系统device file：console、socket、pip

```c++
class TCPChannel{};
class UPDChannle{};
class ConsoleChannel{};
class PipeChannel{};
class FifoChannel{};
class SignalChannel{};
class TimerChannel{};

class Selector{};
enum ChannelOperation{READ, WRITE, ACCEPT, CONNECT, SIGNAL, TIMEOUT};
```

## 示例

```c++
SocketChannel ch;
Selector selector;
ch.registry(selector, Channel::Operation::READ);

selector.select();
```
