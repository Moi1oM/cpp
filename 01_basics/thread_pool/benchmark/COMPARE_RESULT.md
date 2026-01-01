# ⚔️ ThreadPool Performance Battle
### System Spec
- **Date**: Fri Jan  2 01:36:03 KST 2026
- **CPU**: Apple M4 Pro

### Benchmark Result
| Implementation | Tasks | Time |
|---|---|---|
| Standard (Mutex) | 1000000 | 776 ms |
| HFT (SpinLock)   | 1000000 | 906 ms |

> **결과:** SpinLock 방식이 약 **0.86배** 더 빠릅니다.

