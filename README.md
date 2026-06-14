# Flight Price Tracker（航班價格追蹤系統）

大學期末專題 — 使用 C++17 開發的航班價格追蹤網站。

**Repository：** https://github.com/Jerrypppppp/FlightPriceTracker

---

## 快速開始（Clone 與安裝）

### 1. 克隆專案

```bash
git clone https://github.com/Jerrypppppp/FlightPriceTracker.git
cd FlightPriceTracker
```

### 2. 安裝相依套件（macOS）

需先安裝 [Homebrew](https://brew.sh/)，再執行：

```bash
brew install cmake asio
```

| 套件 | 用途 |
|------|------|
| `cmake` | 建置工具 |
| `asio` | Crow 框架網路依賴 |
| `sqlite3` | macOS 通常已內建，無需額外安裝 |

> 首次執行 `cmake ..` 時會自動下載 **Crow** 與 **nlohmann/json**，需保持網路連線，約需 5～10 分鐘。

### 3. 編譯

```bash
mkdir build
cd build
cmake ..
make
```

編譯成功後會產生可執行檔 `FlightPriceTracker`。

### 4. 執行

```bash
./FlightPriceTracker
```

終端機出現以下訊息即代表啟動成功：

```
Flight Price Tracker running at http://localhost:8080
```

### 5. 開啟網頁

在瀏覽器開啟：

**http://localhost:8080**

| 頁面 | 網址 |
|------|------|
| Dashboard | http://localhost:8080 |
| 航班管理 | http://localhost:8080/flights |
| 價格分析 | http://localhost:8080/analysis |
| 統計 | http://localhost:8080/statistics |

### 6. 停止伺服器

在執行 `./FlightPriceTracker` 的終端機按 **`Ctrl + C`**。

---

## 常見問題

**Q：`cmake: command not found`**  
→ 執行 `brew install cmake`

**Q：編譯時出現 `Could NOT find asio`**  
→ 執行 `brew install asio` 後重新 `cmake ..`

**Q：Port 8080 已被占用**  
→ 關閉占用 8080 的程式，或修改 `main.cpp` 中的 port 後重新編譯

**Q：資料存在哪？**  
→ 首次執行會在 `build/SQLite/flights.db` 建立資料庫（需在 `build/` 目錄下執行程式）

---

## 功能特色

- **Dashboard**：航班總數、平均價格、最低價、優惠通知、優惠排行榜
- **航班管理**：新增、編輯、刪除、搜尋、排序
- **價格歷史**：每次更新價格自動記錄
- **價格分析**：最低價、最高價、平均價、跌幅計算
- **優惠偵測**：目前價格 < 平均價格 × 0.8 標記為特價
- **智慧警報**：歷史新低、連續下跌（C++ AnalysisEngine 規則引擎）
- **價格趨勢圖**：Analysis 頁面 Chart.js 視覺化 SQLite 歷史資料
- **統計頁面**：完整系統統計資訊

## 技術棧

| 層級 | 技術 |
|------|------|
| 後端 | C++17, Crow Framework, SQLite, nlohmann/json |
| 前端 | HTML5, Bootstrap 5, JavaScript, Fetch API, Chart.js |

## 系統需求

- macOS 12+（Apple Silicon M 系列 / Intel 皆可）
- CMake 3.16+
- C++17 編譯器（Xcode Command Line Tools）
- 網路連線（首次編譯）

安裝 Xcode Command Line Tools（若尚未安裝）：

```bash
xcode-select --install
```

## 專案結構

```
FlightPriceTracker/
├── main.cpp                  # 程式進入點
├── CMakeLists.txt
├── controllers/              # REST API 路由
├── models/                   # Flight, PriceHistory 類別
├── services/                 # FlightManager, AnalysisEngine
├── database/                 # SQLite 初始化
├── templates/                # HTML 頁面
├── static/                   # CSS, JS
└── SQLite/                   # 資料庫目錄（執行時自動建立）
```

## REST API

| 方法 | 路徑 | 說明 |
|------|------|------|
| GET | `/api/flights` | 取得所有航班（支援 `?search=`、`?sort=`） |
| POST | `/api/flights` | 新增航班 |
| PUT | `/api/flights/{id}` | 更新航班 |
| DELETE | `/api/flights/{id}` | 刪除航班 |
| GET | `/api/flights/{id}/history` | 取得價格歷史 |
| GET | `/api/flights/{id}/analysis` | 取得價格分析 |
| GET | `/api/stats` | 取得統計資料 |

## 類別設計

- **Flight** — 航班資料模型，含 `updatePrice()`、`toJson()`
- **PriceHistory** — 價格歷史紀錄
- **FlightManager** — CRUD、搜尋、排序
- **AnalysisEngine** — 最低/最高/平均價、優惠偵測

## 模擬資料

首次啟動會自動建立 6 筆示範航班（含 CI100 價格歷史：8500 → 8200 → 6900）。  
**資料為模擬資料，非真實航空公司 API。**

## 授權

本專案為教學用途。
