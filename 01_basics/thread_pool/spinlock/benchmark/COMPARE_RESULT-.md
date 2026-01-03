# ⚔️ ThreadPool Performance Battle
### System Spec
- **Date**: Fri Jan  2 01:26:37 KST 2026
- **CPU**: Apple M4 Pro

### Benchmark Result
| Implementation | Tasks | Time |
|---|---|---|
| Standard (Mutex) | 1000000 | 836 ms |
| HFT (SpinLock)   | 1000000 | 2849 ms |

> **결과:** SpinLock 방식이 약 **0.27배** 더 빠릅니다.

---

현대적인 Mutex는 바로 Thread를 재워버리는게 아니라 조금 돌다가 재워버린다고 함.
Task가 너무 간단해서 오히려 SpinLock이 더 오래걸리는 상황이 나온 것 같음.