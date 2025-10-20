# Pipo - When2Meet Clone

REST API сервис для планирования встреч (аналог When2Meet).

## Требования

- CMake 3.14+
- C++20 компилятор
- vcpkg (для управления зависимостями)
- Docker и Docker Compose (для БД)

## Быстрый старт

### 1. Установка vcpkg (если еще не установлен)

```powershell
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
$env:VCPKG_ROOT = (Get-Location).Path
```

### 2. Запуск базы данных

```powershell
docker-compose up -d
```

Это запустит PostgreSQL и применит миграции Liquibase.

### 3. Сборка проекта

**Автоматическая сборка:**
```powershell
.\build.ps1
```

**Ручная сборка:**
```powershell
vcpkg install
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake
cmake --build build
```

### 4. Запуск сервера

**Автоматический запуск:**
```powershell
.\start.ps1
```

**Ручной запуск:**
```powershell
.\build\Debug\pipo-hse.exe
```

Сервер запустится на `http://localhost:8080`

### 5. Тестирование API

```powershell
.\test-api.ps1
```

## API Endpoints

### Пользователи

#### Создать пользователя
```http
POST /api/users
Content-Type: application/json

{
  "username": "johndoe",
  "email": "john@example.com",
  "password": "password123",
  "first_name": "John",
  "last_name": "Doe"
}
```

#### Получить всех пользователей
```http
GET /api/users
```

#### Получить пользователя по ID
```http
GET /api/users/{uuid}
```

## Структура проекта

```
.
├── db/
│   └── changelog/
│       ├── db.changelog-master.yaml
│       └── changes/
│           ├── 001-create-user-table.yaml
│           └── 001-create-user-table.sql
├── include/
│   ├── server.h
│   ├── db/
│   │   └── database.h
│   └── handlers/
│       └── user_handler.h
├── src/
│   ├── main.cpp
│   ├── server.cpp
│   ├── db/
│   │   └── database.cpp
│   └── handlers/
│       └── user_handler.cpp
├── tests/
├── CMakeLists.txt
├── docker-compose.yml
├── vcpkg.json
├── build.ps1
├── start.ps1
└── test-api.ps1
```

## Разработка

### Запуск тестов

```powershell
.\build\Debug\tests_run.exe
```

### Остановка БД

```powershell
docker-compose down
```

### Просмотр логов БД

```powershell
docker-compose logs -f postgres
```

## Технологии

- **C++20** - язык программирования
- **Boost.Beast** - HTTP сервер
- **Boost.Asio** - асинхронный I/O
- **nlohmann/json** - работа с JSON
- **libpqxx** - PostgreSQL клиент
- **PostgreSQL** - база данных
- **Liquibase** - миграции БД
- **Docker** - контейнеризация


