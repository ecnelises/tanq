#ifndef SHELL_H
#define SHELL_H

/* 初始化控制台 */
void init_shell(void);

/* 响应按键操作 */
void shell_key_handler(unsigned k);

/* 执行 shell 命令逻辑 */
void execute(char *command);

#endif /* SHELL_H */
