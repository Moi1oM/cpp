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

