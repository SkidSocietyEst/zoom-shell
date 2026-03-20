/**
 * @file zoom_shell_ai_bridge.c
 * @brief 可选 AI / HTTP 桥接：将一行文本 POST 到用户配置的 URL
 *
 * 不包含 TLS/HTTP 实现。在端口层调用 zoom_ai_bridge_set_post() 注册回调。
 * 可与主机侧 OpenClaw 等通过自建网关协作，见 README「与 OpenClaw / Claw 生态协作」。
 */

#include "zoom_shell.h"

#if ZOOM_USING_AI_BRIDGE

void zoom_ai_bridge_set_post(zoom_shell_t *shell, zoom_ai_http_post_fn_t fn)
{
    if (!shell) return;
    shell->ai_bridge.http_post = fn;
}

static int cmd_ai_url(zoom_shell_t *shell, int argc, char *argv[])
{
    if (argc < 1) {
        zoom_error(shell, "Usage: ai url <http(s)://host/path>\r\n");
        return -1;
    }
    uint16_t i = 0;
    const char *s = argv[0];
    while (s[i] && i < ZOOM_AI_URL_MAX - 1) {
        shell->ai_bridge.url[i] = s[i];
        i++;
    }
    shell->ai_bridge.url[i] = '\0';
    zoom_ok(shell, "AI endpoint URL set\r\n");
    return 0;
}

static int cmd_ai_status(zoom_shell_t *shell, int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    zoom_printf(shell, "  URL:       %s\r\n",
                shell->ai_bridge.url[0] ? shell->ai_bridge.url : "(not set)");
    zoom_printf(shell, "  HTTP POST: %s\r\n",
                shell->ai_bridge.http_post ? "registered" : "(not set — call zoom_ai_bridge_set_post)");
    return 0;
}

static int cmd_ai_ask(zoom_shell_t *shell, int argc, char *argv[])
{
    if (argc < 1) {
        zoom_error(shell, "Usage: ai ask <text...>\r\n");
        return -1;
    }
    if (!shell->ai_bridge.url[0]) {
        zoom_error(shell, "Set URL first: ai url <url>\r\n");
        return -1;
    }
    if (!shell->ai_bridge.http_post) {
        zoom_error(shell, "Register HTTP POST in port: zoom_ai_bridge_set_post()\r\n");
        return -1;
    }

    char body[ZOOM_CMD_MAX_LENGTH];
    int pos = 0;
    for (int a = 0; a < argc && pos < (int)sizeof(body) - 2; a++) {
        if (a > 0 && pos < (int)sizeof(body) - 1)
            body[pos++] = ' ';
        const char *p = argv[a];
        while (*p && pos < (int)sizeof(body) - 1)
            body[pos++] = *p++;
    }
    body[pos] = '\0';

    char resp[ZOOM_AI_RESP_MAX];
    int n = shell->ai_bridge.http_post(shell, shell->ai_bridge.url, body,
                                      (uint16_t)pos, resp,
                                      (uint16_t)sizeof(resp));
    if (n < 0) {
        zoom_error(shell, "HTTP POST failed (%d)\r\n", n);
        return -1;
    }
    if (n >= (int)sizeof(resp))
        n = (int)sizeof(resp) - 1;
    resp[n] = '\0';
    zoom_printf(shell, "%s\r\n", resp);
    return 0;
}

ZOOM_SUBCMD_SET(sub_ai,
    ZOOM_SUBCMD(url,    cmd_ai_url,    "Set POST endpoint URL"),
    ZOOM_SUBCMD(ask,    cmd_ai_ask,    "POST plain text body to URL"),
    ZOOM_SUBCMD(status, cmd_ai_status, "Show URL and callback registration"),
);

#if ZOOM_USING_CMD_EXPORT
ZOOM_EXPORT_CMD_WITH_SUB(ai, sub_ai, "AI HTTP bridge (user implements POST)",
                          ZOOM_ATTR_DEFAULT, ZOOM_USER_GUEST);
#endif

#endif /* ZOOM_USING_AI_BRIDGE */
