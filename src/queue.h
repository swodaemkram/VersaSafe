#include <functional>
typedef std::function<void(int)> cb_t;

extern void InitEventDriver(void);
extern uint32_t AddCallBack(const cb_t &cb);
extern void ExecCallBacks(void);
extern void RemoveCallBack(uint32_t);


