# создаём контейнер build
FROM gcc:11.4 as build

# Настраиваем зависимости
RUN apt update && \
    apt install -y \
      python3-pip \
      cmake \
    && \
    pip install conan==1.62.0

# Собираем либы через conan
COPY conanfile.txt /app/
RUN mkdir /app/build && cd /app/build && \
    conan install .. --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11

# Переносим в docker контейнер исходники
COPY ./src /app/src
COPY ./tests /app/tests
COPY CMakeLists.txt /app/

# Сборка проекта
RUN cd /app/build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build .
    
# Создаём контейнер run
FROM ubuntu:22.04 as run

# Создадим пользователя www
RUN groupadd -r www && useradd -r -g www www
USER www

# Скопируем приложение со сборочного контейнера в директорию /app.
COPY --from=build /app/build/bin/game_server /app/
COPY ./data /app/data
COPY ./static /app/static

# Запускаем игровой сервер (точка входа /app/game_server с параметроми)
ENTRYPOINT ["/app/game_server", "--randomize-spawn-points", "--config-file" ,"/app/data/config.json", "--www-root" ,"/app/static"]
#"--save-state-period", "1000", "--state-file", "state.save"
