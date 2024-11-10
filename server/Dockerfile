FROM alpine:latest

WORKDIR /app

COPY ./libraries /app/libraries

COPY ./server ./server

RUN apk add --no-cache make clang libpq postgresql-dev

WORKDIR /app/server

RUN make

EXPOSE 6969

CMD ["./uchat_server"]