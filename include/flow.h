#define FLOW_H_INCLUDED

/* Common variables */
struct flow
{
    char log[256];  // Log string
};

struct flow app;

void err();
void wlog(int log_type, int to_stdout);
