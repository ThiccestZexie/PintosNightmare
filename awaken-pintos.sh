docker build -t pintos . && docker run --rm -it -p 1234:1234 -v ./pintos:/pintos pintos
