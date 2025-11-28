# ChatLearn 项目

一个基于 C++ 和 Node.js 的聊天系统，包含网关服务器、验证服务器和客户端。用于自学C++，项目来源:[b站up主 恋恋风辰zack](https://space.bilibili.com/271469206/channel/collectiondetail?sid=2323318&spm_id_from=333.788.0.0 "b站up主 恋恋风辰zack")

## 项目结构

```
chat_learn/
├── GateServer/          # C++ 网关服务器
├── VerifyServer/        # Node.js 验证服务器  
└── zhaoy7chat/          # Qt 客户端
```

## 组件说明

### 1. GateServer (C++ 网关服务器)
- 使用 Visual Studio 开发
- 处理客户端连接和消息路由
- 基于 gRPC 进行服务间通信

### 2. VerifyServer (Node.js 验证服务器)
- 用户身份验证和权限管理
- 邮箱验证功能
- RESTful API 设计

### 3. zhaoy7chat (Qt 客户端)
- 跨平台桌面聊天客户端
- 用户登录/注册界面
- 实时消息通信



## 技术栈

- **后端**: C++, Node.js, gRPC, Protobuf
- **前端**: Qt, C++
- **通信**: HTTP, gRPC
