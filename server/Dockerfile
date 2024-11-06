FROM silkeh/clang:latest

WORKDIR /app

COPY . .

RUN make

EXPOSE 6969

CMD ["./uchat_server"]