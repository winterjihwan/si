<div align="center">

### Snapshot Isolation Simulator

</div>

## 📝 소개

트랜잭션 격리 수준 중 하나인 Snapshot Isolation을 시뮬레이션하는 프로젝트입니다. 테이블, 디스크, 트랜잭션 관리 및 리커버리 메커니즘을 포함하며, 버전 관리 기반으로 동시성 제어 및 커밋/어보트 로직을 구현하였습니다. C 언어로 개발되었으며, 실습을 통해 Snapshot Isolation의 핵심 원리를 직접 실현하는 데 목적을 두었습니다.

<br />

## 🌊 흐름

1. **초기화**  
   `main.c` → 테이블 및 리소스 생성

2. **트랜잭션 생성**  
   `tx.c` → 트랜잭션을 생성하고 워크스페이스를 스냅샷으로 복제

3. **트랜잭션 조작**  
   `tx.c` → `read()`, `write()`, `commit()`, `abort()` 호출

4. **스냅샷 생성**  
   `snapshot.c` → 테이블로부터 읽기 일관성을 보장하는 스냅샷 생성

5. **로그 기록**  
   `recovery.c` → 트랜잭션의 read/write/commit/abort을 로그로 기록

6. **충돌 감지 및 복구**  
   `tx.c`, `recovery.c` → SI 프로토콜에 따른 충돌 감지 및 복구 절차 실행

7. **결과 출력**  
   `disk_table_dump()` 및 `tx_schedule_dump()` 호출로 상태 출력

<br />

## ⚙ 기술

<img src="./images/c.png" alt="C Language" width="50"/>

<br />

## 🐛 트러블슈팅

<br />

## 🚀 사용법

```bash
make main
./main
```

<br />

## 📚 참고자료

**Database System Concepts** (7th ed.) by Abraham Silberschatz et al.
