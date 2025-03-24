<div align="center">

### Snapshot Isolation Simulator

</div>

## 📝 소개

Snapshot Isolation 알고리즘을 직접 구현해본 프로젝트. 데이터베이스 시스템 수업 중 교재에서 소개된 Snapshot Isolation의 동작 방식을 깊이 이해하고 싶어 테이블, 디스크, 트랜잭션 관리, 리커버리 메커니즘을 포함한 버전 관리 기반 동시성 제어를 C 언어로 구현했다.

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

## 💡 느낀점

PostgreSQL의 병행 제어 방식을 학습하며 DBMS의 내부 동작 원리를 체감할 수 있었고, 투페이즈 락킹을 비롯해 다양한 병행 제어 기법도 직접 구현해보고 싶은 동기를 얻었다. 또한, 실행된 데이터베이스 쿼리를 분석하고 더 효율적인 쿼리로 자동 변환하는 최적화 도구도 추후 직접 만들어보고 싶다.

<br />

## 📚 참고자료

**Database System Concepts** (7th ed.) by Abraham Silberschatz et al.
