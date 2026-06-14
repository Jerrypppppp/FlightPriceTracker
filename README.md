# Flight Price Tracker（航班價格追蹤系統）

大學期末專題 — 使用 C++17 開發的航班價格追蹤網站。

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
| 前端 | HTML5, Bootstrap 5, JavaScript, Fetch API |

## 系統需求

- macOS (Apple Silicon / Intel)
- CMake 3.16+
- SQLite3（macOS 內建）
- 網路連線（首次編譯需下載 Crow 與 nlohmann/json）

## 編譯與執行

```bash
mkdir build
cd build
cmake ..
make
./FlightPriceTracker
```

啟動後在瀏覽器開啟：

**http://localhost:8080**

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
└── SQLite/                   # 資料庫檔案（執行時自動建立）
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

## 授權

本專案為教學用途。
