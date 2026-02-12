# YunoEngine

<img width="512" height="512" alt="image" src="https://github.com/user-attachments/assets/9017a966-1fea-4252-b9a6-11ef7e3df2cb" />

인재원 7기 4쿼터 미니 프로젝트에 사용할 엔진입니다.


[엔진 안내](https://www.notion.so/4Q-2ce9357db9428096a44ac341eeb05416) 


## 로컬 네트워크(공유기)에서 2대 PC로 플레이하기

기본값은 클라이언트가 `127.0.0.1:9000`으로 접속합니다.
이제 아래 환경변수로 서버 주소/포트를 바꿀 수 있습니다.

- `YUNO_SERVER_HOST` (기본값: `127.0.0.1`)
- `YUNO_SERVER_PORT` (기본값: `9000`)

### 1) 서버 PC
1. `YunoServer` 실행
2. 방화벽에서 TCP `9000` 인바운드 허용
3. 서버 PC의 로컬 IP 확인 (`ipconfig`)
   - 예: `192.168.0.23`

### 2) 클라이언트 PC 2대
각 PC에서 `YunoGame` 실행 전에 환경변수 설정:

- CMD
```bat
set YUNO_SERVER_HOST=192.168.0.23
set YUNO_SERVER_PORT=9000
set YUNO_USER_ID=1
YunoGame.exe
```

다른 PC는 `YUNO_USER_ID`만 다른 값으로 실행:

```bat
set YUNO_SERVER_HOST=192.168.0.23
set YUNO_SERVER_PORT=9000
set YUNO_USER_ID=2
YunoGame.exe
```

### 참고
- 두 PC 모두 같은 공유기 대역(예: `192.168.0.x`)이어야 합니다.
- 접속이 안 되면 방화벽/백신의 네트워크 차단 여부를 먼저 확인하세요.