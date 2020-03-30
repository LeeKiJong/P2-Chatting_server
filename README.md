# Chatting_server
## 목차
* 프로그램 설명
* 포함 목록
* 동작 과정
* 기능 소개
* 오류 해결책
* 동작 화면

## 프로그램 설명
위 프로그램은 소켓을 사용해 만든 채팅 프로그램입니다. 귓속말 기능, 채팅 기능, 사용자 목록 출력 기능을 가지고 있습니다.

## 포함 목록
다운로드 목록에는 다음과 같은 디렉토리와 파일이 있으며 공통 에셋을 논리적으로 그룹화하고 유사 및 축소 변형을 모두 제공합니다.  
볼 수 있는 목록:
* Chatting_server/
   * Client
      * Client
         * Client.vcxproj
         * Client.vcxproj.filters
         * Client.c
      * Client.Sln
   * Server
      * Server
         * Server.vcxproj
         * Server.vcxproj.filters
         * Server.c
      * Server.Sln

## 동작 과정
1. Clone the repo: `git clone https://github.com/LeeKiJong/Chatting_server.git` 
2. Open the 2+ CMD Prompt
3. server cmd: `location.../Server <Port>`
4. Client cmd: `location.../Client <IP> <Port> <ID>`

## 기능 소개
1. Whisper Function: `/to <ID> <Msg>`
2. User List Function: `/list`

## 오류 해결책
Setting Option  
![1](https://user-images.githubusercontent.com/52438368/65368884-772f6f00-dc81-11e9-87b3-d89dc1af1dac.PNG)

## 동작 화면
* 채팅
![image](https://user-images.githubusercontent.com/52438368/67141192-08ebc580-f29c-11e9-8684-d80781f7f3f3.png)
* 사용자 목록 출력
![image](https://user-images.githubusercontent.com/52438368/67141207-24ef6700-f29c-11e9-935f-db0a469b1a73.png)
* 귓속말(ID2 --> ID3)
![image](https://user-images.githubusercontent.com/52438368/67141352-c24a9b00-f29c-11e9-9458-1d264036524c.png)


