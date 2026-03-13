
# 创建数据库和表结构的 SQL 脚本

-- 创建数据库 NexFile，如果不存在则创建，并设置默认字符集和排序规则
CREATE DATABASE IF NOT EXISTS NexFile DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE NexFile;

-- 创建用户信息表，包含用户 ID、用户名、盐值、加密密码、当前工作目录和删除标记
CREATE TABLE user_info (
    user_id INT AUTO_INCREMENT PRIMARY KEY COMMENT '用户 ID',
    user_name VARCHAR(20) NOT NULL COMMENT '用户名',
    salt CHAR(32) NOT NULL COMMENT '盐值',
    crypt_passwd VARCHAR(64) NOT NULL COMMENT '加密密码',
    home_dir VARCHAR(255) COMMENT '当前工作目录',
    is_deleted TINYINT DEFAULT 0 COMMENT '是否删除 (0:否，1:是)'
) COMMENT='用户信息表';

-- 清空用户信息表
TRUNCATE TABLE user_info;

-- 删除表结构
DROP TABLE user_info;

-- 创建用户文件信息表，包含文件 ID、父目录 ID、文件名、所属用户 ID、文件 MD5 校验码、文件大小、类型、删除标记、创建时间和更新时间
CREATE TABLE user_file (
    file_id INT AUTO_INCREMENT PRIMARY KEY COMMENT '文件 ID 主键',
    parent_id INT DEFAULT 0 COMMENT '父目录 ID (0 表示根目录)',
    filename VARCHAR(255) NOT NULL COMMENT '文件名',
    owner_id INT NOT NULL COMMENT '所属用户 ID',
    file_md5 CHAR(32) COMMENT '文件 MD5 校验码',
    file_size BIGINT DEFAULT 0 COMMENT '文件大小 (字节)',
    type TINYINT DEFAULT 1 COMMENT '类型 (0:目录，1:文件)',
    is_deleted TINYINT DEFAULT 0 COMMENT '删除标记 (0:正常，1:已删除)',
    create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    update_time DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间'
) COMMENT='用户文件信息表';

-- 清空用户文件信息表
TRUNCATE TABLE user_file;

-- 删除表结构
DROP TABLE user_file;
