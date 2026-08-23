#pragma once
#include "log.hpp"
#include "core.hpp"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h> 
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <linux/io_uring.h>
#include <sys/stat.h>
#include <atomic>
#include <sys/uio.h>
#include <sys/sendfile.h>

namespace Koral {
using argv_t = char**;
using addr_t = sockaddr_in;
using addrs_t = sockaddr_storage;
#define localhost 0x0100007f
#define event_t epoll_event
#define IIPv6 IPPROTO_IPV6
#define IPv6O IPV6_V6ONLY
#define sol_sock SOL_SOCKET
#define IPv6 AF_INET6
#define IPv4 AF_INET


struct sock_body {
  auto S() { return SOCK_STREAM; } // TCP
  auto D() { return SOCK_DGRAM;  } // UDP
};
inline sock_body SOCK;
  
namespace sock {
  inline ssize_t send(int fd, std::string_view text, int flags = MSG_NOSIGNAL) {
      return ::send(fd, text.data(), text.size(), flags);
  }
  template <size_t N>
  inline ssize_t recv(int fd, char (&buf)[N], int flags = 0) {
      return ::recv(fd, buf, N, flags);
  }

  template <size_t N>
  inline ssize_t read(int fd, char (&buf)[N]) {
      return ::read(fd, buf, N);
  }

  inline ssize_t write(int fd, std::string_view text) {
      return ::write(fd, text.data(), text.size());
  }

  
  inline int acpt2(int server_fd, addr_t addr, int flags) {
    socklen_t client_len = sizeof(addr);
    
    return accept4(server_fd, reinterpret_cast<sockaddr*>(&addr), &client_len, flags);

  }
  
  inline int acpt2(int server_fd, int flags) {
    return accept4(server_fd, nullptr, nullptr, flags);

  }
  
  inline int conn(int fd, sockaddr_storage& addr) {
    return connect(fd, reinterpret_cast<const struct sockaddr*>(&addr), sizeof(addr));
  } 
  
  inline int acpt(int server_fd, addr_t addr) {
    socklen_t client_len = sizeof(addr);
    
    return accept(server_fd, reinterpret_cast<sockaddr*>(&addr), &client_len);
  }
  
  inline int acpt(int server_fd) {
    return accept(server_fd, nullptr, nullptr);
  }
  
  template <int domain>
  inline auto open(int type) {
    return socket(domain, type, 0);
  }
  
  template <typename Func>
  void OnFail(int num, Func func) {
    if (num < 0) { func(); }
  }
 
  template <typename Func>
  void OnSucc(int num, Func func) {
    if (num >= 0) { func(); }
  }
  
  inline void opt(int fd, int sock, int flag) {
    int opt_val = 1;
    setsockopt(fd, sock, flag, &opt_val, sizeof(opt_val));

    int no = 0;
    setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no));
  }

  template <int domain, typename T>
  inline sockaddr_storage craddr(uint16_t port, const T& ADDR) {
    sockaddr_storage storage{};
    std::memset(&storage, 0, sizeof(storage));

    if constexpr (domain == IPv6) {
        auto* addr6 = reinterpret_cast<sockaddr_in6*>(&storage);
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = htons(port);
        addr6->sin6_addr = ADDR; 
    } else {
        auto* addr4 = reinterpret_cast<sockaddr_in*>(&storage);
        addr4->sin_family = AF_INET;
        addr4->sin_port = htons(port);
        addr4->sin_addr.s_addr = ADDR; 
    }
    return storage;
  }
  
  auto bind(int& arg, sockaddr_storage& addr) {
    return ::bind(arg, reinterpret_cast<const struct sockaddr*>(&addr), sizeof(addr));
  }
  
  template <int domain, typename T>
  int btrap(int type, uint16_t port, const T& addr, int count, int wsock, int add) {
    int fd = Koral::sock::open<domain>(type);
    if (fd < 0) {
      Koral::out.error("sock::btrap: Can't create socket");
      return -1;
    }
    Koral::sock::opt(fd, wsock, add);
    addrs_t addrs = Koral::sock::craddr<domain>(port, addr);
    
    if (sock::bind(fd, addrs) < 0) {
      Koral::out.error(f("Can't bind address: {} and port: {}.",addr, port));
      return -1;
    }
    
    if (listen(fd, count) < 0) {
        out.error("Can't listen.");
        return -1;
    }
    return fd;
  }
}

namespace epoll {
  template <typename T>
  auto open(const T arg) {
    return epoll_create1(arg);
  }
  
  inline auto event(int arg, int num) {
    struct epoll_event ep{};
    ep.events = arg;
    ep.data.fd = num;
    return ep;
  }
  
  inline auto ctl(int epoll_fd, const int num, int listen_fd, epoll_event& ev) {
    return epoll_ctl(epoll_fd, num, listen_fd, &ev);
  }
  
  inline auto ectl(int arg, const int& num, const int& epoll_fd, const int nu) {
    epoll_event epoll_d = epoll::event(arg, num);
    return epoll::ctl(epoll_fd,nu, num, epoll_d);
  }
  
  inline auto wait(int epoll_fd, epoll_event* events, int max_events, int timeout_ms) { 
    return epoll_wait(epoll_fd, events, max_events, timeout_ms); 
  }
  
  template <typename Func>
  void OnFail(int num, Func func) {
    if (num < 0) { func(); }
  }
 
  template <typename Func>
  void OnSucc(int num, Func func) {
    if (num >= 0) { func(); }
  }
  
}

namespace iu {
  struct SetupObjects {
    int ring_fd;
    struct io_uring_params params;
    void *sq_ptr;
    void *cq_ptr;
    struct io_uring_sqe *sqes;
  };
  auto setup(int ringnum) {
    SetupObjects ctx; 

    std::memset(&ctx.params, 0, sizeof(ctx.params));
    
    ctx.ring_fd = static_cast<int>(syscall(__NR_io_uring_setup, ringnum, &ctx.params));
    if (ctx.ring_fd < 0) {
      raise("[ERROR] Can't create setup.");
      ctx.ring_fd = -1;
      return ctx;
    }
    
    size_t sq_ring_sz = ctx.params.sq_off.array + ctx.params.sq_entries * sizeof(unsigned);
    size_t cq_ring_sz = ctx.params.cq_off.cqes + ctx.params.cq_entries * sizeof(struct io_uring_cqe);
    size_t sqes_sz    = ctx.params.sq_entries * sizeof(struct io_uring_sqe);
      
    ctx.sq_ptr = mmap(0, sq_ring_sz, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, ctx.ring_fd, IORING_OFF_SQ_RING);
    ctx.cq_ptr = mmap(0, cq_ring_sz, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, ctx.ring_fd, IORING_OFF_CQ_RING);
    ctx.sqes   = (struct io_uring_sqe *)mmap(0, sqes_sz, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, ctx.ring_fd, IORING_OFF_SQES);

    if (ctx.sq_ptr == MAP_FAILED || ctx.cq_ptr == MAP_FAILED || ctx.sqes == MAP_FAILED) {
      raise("[ERROR] Can't allocate memory");
      ctx.ring_fd = -1;
      return ctx;
    }

    return ctx; 
}

  auto enter(SetupObjects& ctx, int opcd, int id, int num, int num2, int num3, int num4) {
    unsigned *sq_tail  = (unsigned*)((char*)ctx.sq_ptr + ctx.params.sq_off.tail);
    unsigned *sq_mask  = (unsigned*)((char*)ctx.sq_ptr + ctx.params.sq_off.ring_mask);
    unsigned *sq_array = (unsigned*)((char*)ctx.sq_ptr + ctx.params.sq_off.array);
    
    unsigned tail = *sq_tail;
    unsigned index = tail & *sq_mask;
    
    struct io_uring_sqe *sqe = &ctx.sqes[index];
    std::memset(sqe, 0, sizeof(*sqe));
    sqe->opcode = static_cast<__u8>(opcd);
    sqe->user_data = id;
    
    sq_array[index] = index;
    std::atomic_thread_fence(std::memory_order_release);
    *sq_tail = tail + 1;
    
    return syscall(__NR_io_uring_enter, ctx.ring_fd, num, num2, num3, nullptr, num4);
  }

  template <typename Func>
  void OnFail(SetupObjects num, Func func) {
    if (num.ring_fd < 0) { func(); }
  }
 
  template <typename Func>
  void OnSucc(SetupObjects num, Func func) {
    if (num.ring_fd >= 0) { func(); }
  }
}

namespace zero {
  template <typename T>
  auto sendf(int out_fd, int in_fd, T* offset, size_t count) {
      return sendfile(out_fd, in_fd, offset, count);
  }
  auto splice(int fd_in, int fd_out, int pipe_write_fd, int pipe_read_fd, size_t count) {
      ssize_t spliced_in = ::splice(fd_in, nullptr, pipe_write_fd, nullptr, count, SPLICE_F_MOVE | SPLICE_F_NONBLOCK);
      if (spliced_in <= 0) return spliced_in;
      return ::splice(pipe_read_fd, nullptr, fd_out, nullptr, spliced_in, SPLICE_F_MOVE | SPLICE_F_NONBLOCK);
  }
}

namespace vec {
  template <typename... Args>
  inline auto write(int fd, Args&... arg) {
    struct iovec iov[] = {
      { &arg, sizeof(arg) }...
    };
    return writev(fd, iov, sizeof...(Args));
  }
  
  template <typename... Args>
  inline auto read(int fd, Args&... arg) {
    struct iovec iov[] = {
      { &arg, sizeof(arg) }...
    };
    return readv(fd, iov, sizeof...(Args));
  }
}

namespace multi {
  struct MultiContext {
    std::vector<char> buffers;
    std::vector<sockaddr_in> addrs;
    std::vector<sockaddr_in6> addrs6;
    std::vector<struct mmsghdr> msgs; 
    std::vector<struct iovec> iov;
  };
  
  template <int domain>
  auto init(int MAX_PACKETS, int BUFFER_SIZE) {
    MultiContext ctx;
    ctx.msgs.resize(MAX_PACKETS);
    ctx.buffers.resize(MAX_PACKETS * BUFFER_SIZE); 
    ctx.iov.resize(MAX_PACKETS);
    
    if constexpr (domain == IPv4) {
      ctx.addrs.resize(MAX_PACKETS);
      
      for (int i = 0; i < MAX_PACKETS; ++i) {
      ctx.iov[i].iov_base = ctx.buffers.data() + (i * BUFFER_SIZE);
      ctx.iov[i].iov_len  = BUFFER_SIZE;
      
      
      ctx.msgs[i].msg_hdr.msg_name = &ctx.addrs[i];
      ctx.msgs[i].msg_hdr.msg_namelen = sizeof(ctx.addrs[i]);
      ctx.msgs[i].msg_hdr.msg_iov = &ctx.iov[i];
      ctx.msgs[i].msg_hdr.msg_iovlen = 1;
    }
    } else {
      ctx.addrs6.resize(MAX_PACKETS); 
      
      for (int i = 0; i < MAX_PACKETS; ++i) {
      ctx.iov[i].iov_base = ctx.buffers.data() + (i * BUFFER_SIZE);
      ctx.iov[i].iov_len  = BUFFER_SIZE;
      
      
      ctx.msgs[i].msg_hdr.msg_name = &ctx.addrs6[i];
      ctx.msgs[i].msg_hdr.msg_namelen = sizeof(ctx.addrs6[i]);
      ctx.msgs[i].msg_hdr.msg_iov = &ctx.iov[i];
      ctx.msgs[i].msg_hdr.msg_iovlen = 1;
    } }
    return ctx;
  }
  
  
  timespec time(int sec,int nsec = 0) {
    return timespec{sec, nsec};
  }
  
  auto read(int fd, std::vector<struct mmsghdr>& vec, int MAX_PACKETS, int flag = 0, timespec* ptr = nullptr) {
    
    return recvmmsg(fd, vec.data(), MAX_PACKETS, flag, ptr);
  }
  auto write(int fd, MultiContext& vec, int PACKETS_TO_SEND, int flag = 0) {
    return sendmmsg(fd, vec.msgs.data(), PACKETS_TO_SEND, flag);
  }
  
    template <int domain>
  auto reset(MultiContext& ctx, int MAX_PACKETS) {
    if constexpr (domain == IPv4) {
      for (int i = 0; i < MAX_PACKETS; ++i) {
        ctx.msgs[i].msg_hdr.msg_namelen = sizeof(sockaddr_in); 
      }
    } else {
      for (int i = 0; i < MAX_PACKETS; ++i) {
        ctx.msgs[i].msg_hdr.msg_namelen = sizeof(sockaddr_in6); 
      }
    }
  }

}
}