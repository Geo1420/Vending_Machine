#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

// =====================================================
// WiFi
// =====================================================

const char* ssid = "OPPO A74";
const char* password = "1234qwer";


// =====================================================
// UART Arduino Mega <-> ESP32
// =====================================================

#define RX_PIN 16
#define TX_PIN 17


// =====================================================
// Web Server
// =====================================================

WebServer server(80);


// =====================================================
// Variabile
// =====================================================

float temperature = 0.0;
float humidity = 0.0;
float fanTemperatureThreshold = 28.0;

int fanState = 0;
String lastProductCode = "";
String productCodes[] = {"11", "12", "13", "14"};
int productStock[] = {5, 3, 4, 2};
float productPrices[] = {10.0, 12.5, 8.0, 9.0};
String productExpiryDates[] = {"2026-09-15", "2026-10-15", "2026-11-15", "2026-12-15"};

const char* SALES_FILE = "/sales.json";
const char* FAN_THRESHOLD_FILE = "/fan_threshold.json";
const int MAX_SALES_RECORDS = 500;
String salesCodes[MAX_SALES_RECORDS];
float salesPrices[MAX_SALES_RECORDS];
String salesDates[MAX_SALES_RECORDS];
int salesCount = 0;
float totalRevenue = 0.0f;
String periodStart = "2026-09-01";
String periodEnd = "2026-09-30";

const unsigned long MAX_EXECUTION_SLICE_MS = 50;
const char* INVENTORY_FILE = "/products.json";
const int INVENTORY_PRODUCT_COUNT = 4;

// =====================================================
// PAGINA WEB
// =====================================================

const char MAIN_page[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html lang="ro">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Monitorizare Vending Machine</title>
    <style>
        * {
            box-sizing: border-box;
        }

        body {
            margin: 0;
            font-family: Arial, sans-serif;
            background: linear-gradient(135deg, #eaf7ec 0%, #f8eee4 100%);
            text-align: center;
            color: #24472b;
        }

        header {
            background: linear-gradient(135deg, #1d4632 0%, #173722 100%);
            color: white;
            padding: 24px 20px;
            box-shadow: 0 8px 20px rgba(0, 0, 0, 0.12);
        }

        header h1 {
            margin: 0;
            font-size: clamp(30px, 4vw, 34px);
        }

        .container {
            display: flex;
            justify-content: center;
            align-items: stretch;
            gap: 30px;
            margin-top: 36px;
            padding: 20px;
            flex-wrap: wrap;
        }

        .card {
            background: linear-gradient(180deg, #ffffff 0%, #eef8ef 100%);
            width: 280px;
            min-height: 220px;
            padding: 30px;
            border-radius: 20px;
            box-shadow: 0 10px 22px rgba(25, 95, 47, 0.16);
            border: 1px solid rgba(48, 116, 66, 0.2);
        }

        .temperature-card {
            border-top: 4px solid #ed8b31;
        }

        .humidity-card {
            border-top: 4px solid #4bb3cd;
        }

        .fan-card {
            border-top: 4px solid #8260d8;
        }

        .title {
            font-size: 22px;
            margin-bottom: 20px;
            font-weight: 700;
        }

        .value {
            font-size: 48px;
            font-weight: bold;
            color: #173722;
        }

        .unit {
            font-size: 25px;
        }

        .fan-container {
            position: relative;
            width: 150px;
            height: 150px;
            margin: 10px auto;
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .fan {
            position: relative;
            width: 120px;
            height: 120px;
        }

        .fan-center {
            position: absolute;
            left: 50%;
            top: 50%;
            transform: translate(-50%, -50%);
            width: 30px;
            height: 30px;
            background: #444;
            border-radius: 50%;
            z-index: 5;
        }

        .blade {
            position: absolute;
            left: 50%;
            top: 50%;
            width: 25px;
            height: 55px;
            background: #555;
            border-radius: 50% 50% 20% 20%;
            transform-origin: 50% 100%;
        }

        .blade1 {
            transform: translate(-50%, -100%) rotate(0deg);
        }

        .blade2 {
            transform: translate(-50%, -100%) rotate(120deg);
        }

        .blade3 {
            transform: translate(-50%, -100%) rotate(240deg);
        }

        .fan-running {
            animation: rotateFan 0.35s linear infinite;
        }

        @keyframes rotateFan {
            from { transform: rotate(0deg); }
            to { transform: rotate(360deg); }
        }

        .wind {
            position: absolute;
            right: -15px;
            top: 50%;
            transform: translateY(-50%);
            font-size: 28px;
            color: #777;
            opacity: 0;
        }

        .wind-active {
            animation: windAnimation 1s linear infinite;
        }

        @keyframes windAnimation {
            0% {
                transform: translateY(-50%) translateX(20px);
                opacity: 0;
            }
            30% { opacity: 1; }
            70% { opacity: 1; }
            100% {
                transform: translateY(-50%) translateX(70px);
                opacity: 0;
            }
        }

        .fan-status {
            margin-top: 15px;
            font-size: 22px;
            font-weight: bold;
        }

        .fan-on {
            color: #2e7d32;
        }

        .fan-off {
            color: #777;
        }

        .status {
            margin-top: 30px;
            color: #555;
            font-size: 16px;
        }

        .config-panel {
            background: linear-gradient(180deg, #ffffff 0%, #eef9f3 100%);
            width: min(940px, calc(100vw - 40px));
            margin: 30px auto 20px auto;
            padding: 28px;
            border-radius: 20px;
            box-shadow: 0 12px 26px rgba(25, 95, 47, 0.16);
            border: 1px solid rgba(48, 116, 66, 0.14);
        }

        .config-panel .title {
            font-size: 24px;
            margin-bottom: 20px;
            color: #1b563e;
            font-weight: 800;
        }

        .product-config {
            display: flex;
            flex-direction: column;
            gap: 12px;
        }

        .product-config-row {
            display: grid;
            grid-template-columns: 70px 130px 130px 150px 120px;
            gap: 12px;
            align-items: center;
            justify-content: center;
        }

        .product-config-header {
            font-weight: 800;
            color: #234b30;
            font-size: 14px;
            text-align: center;
        }

        .product-code {
            text-align: center;
            font-weight: 800;
            font-size: 21px;
            color: #1f6a45;
        }

        .product-config input {
            width: 120px;
            padding: 8px;
            border-radius: 10px;
            border: 1px solid #bbc9b6;
            text-align: center;
            background: #fdfdfd;
            color: #16391f;
        }

        .product-config button {
            padding: 10px 16px;
            border-radius: 10px;
            border: none;
            background: #2e7d32;
            color: white;
            cursor: pointer;
            font-weight: 700;
            transition: background 0.2s ease, transform 0.2s ease;
        }

        .product-config button:hover {
            background: #256b29;
            transform: translateY(-2px);
        }
    </style>
</head>
<body>

<header>
    <h1>Monitorizare Vending Machine</h1>
</header>

<div class="container">

    <div class="card temperature-card">
        <div class="title">🌡️ Temperatura</div>
        <div class="value">
            <span id="temperature">--</span>
            <span class="unit">°C</span>
        </div>
    </div>

    <div class="card humidity-card">
        <div class="title">💧 Umiditate</div>
        <div class="value">
            <span id="humidity">--</span>
            <span class="unit">%</span>
        </div>
    </div>

    <div class="card fan-card">
        <div class="title">🌀 Ventilator</div>

        <div class="fan-container">
            <div id="fan" class="fan">
                <div class="blade blade1"></div>
                <div class="blade blade2"></div>
                <div class="blade blade3"></div>
                <div class="fan-center"></div>
            </div>

            <div id="wind" class="wind">)))</div>
        </div>

        <div id="fanStatus" class="fan-status fan-off">Ventilator OPRIT</div>
    </div>

</div>

<div class="fan-threshold-panel">
    <div class="title">🌡️ Prag ventilator</div>
    <div class="fan-threshold-row">
        <label for="fanTemperatureThreshold">Temperatura prag (°C)</label>
        <input id="fanTemperatureThreshold" type="number" min="0" max="80" step="0.5" value="28.0">
        <button id="saveFanThreshold">Salveaza prag</button>
    </div>
</div>

<div class="config-panel">
    <div class="title">🧃 Product configuration</div>

    <div class="product-config">

        <div class="product-config-row">
            <div class="product-config-header">Cod</div>
            <div class="product-config-header">Cantitate</div>
            <div class="product-config-header">Pret</div>
            <div class="product-config-header">Expirare</div>
            <div class="product-config-header">Acțiune</div>
        </div>

        <div class="product-config-row">
            <div class="product-code">11</div>
            <input id="stock-11" type="number" min="0" value="5">
            <input id="price-11" type="number" min="0" step="0.01" value="10.00">
            <input id="expiry-11" type="date" value="2026-09-15">
            <button onclick="saveProduct('11')">Salveaza</button>
        </div>

        <div class="product-config-row">
            <div class="product-code">12</div>
            <input id="stock-12" type="number" min="0" value="3">
            <input id="price-12" type="number" min="0" step="0.01" value="12.50">
            <input id="expiry-12" type="date" value="2026-10-15">
            <button onclick="saveProduct('12')">Salveaza</button>
        </div>

        <div class="product-config-row">
            <div class="product-code">13</div>
            <input id="stock-13" type="number" min="0" value="4">
            <input id="price-13" type="number" min="0" step="0.01" value="8.00">
            <input id="expiry-13" type="date" value="2026-11-15">
            <button onclick="saveProduct('13')">Salveaza</button>
        </div>

        <div class="product-config-row">
            <div class="product-code">14</div>
            <input id="stock-14" type="number" min="0" value="2">
            <input id="price-14" type="number" min="0" step="0.01" value="9.00">
            <input id="expiry-14" type="date" value="2026-12-15">
            <button onclick="saveProduct('14')">Salveaza</button>
        </div>

    </div>
</div>

<div class="status">
    Ultima actualizare:
    <span id="updateTime">--</span>
</div>

<script>
function saveProduct(code) {

    const stockInput = document.getElementById("stock-" + code);
    const priceInput = document.getElementById("price-" + code);
    const expiryInput = document.getElementById("expiry-" + code);

    const payload = new URLSearchParams();
    payload.append("code", code);
    payload.append("quantity", stockInput.value);
    payload.append("price", priceInput.value);
    payload.append("expiryDate", expiryInput.value);

    fetch("/setProduct", {
        method: "POST",
        headers: {
            "Content-Type": "application/x-www-form-urlencoded"
        },
        body: payload.toString()
    })
    .then(response => response.json())
    .then(result => {
        console.log("Product updated:", result);
        updateData();
    })
    .catch(error => {
        console.log("Product save error:", error);
    });
}

function updateProductConfig(data) {

    if (!Array.isArray(data.productStock)) {
        return;
    }

    for (let i = 0; i < data.productStock.length; i++) {

        const product = data.productStock[i];
        const stockField = document.getElementById("stock-" + product.code);
        const priceField = document.getElementById("price-" + product.code);
        const expiryField = document.getElementById("expiry-" + product.code);

        if (stockField) {
            stockField.value = product.quantity;
        }

        if (priceField && Array.isArray(data.productPrices)) {
            const priceProduct = data.productPrices.find(item => item.code === product.code);
            if (priceProduct) {
                priceField.value = priceProduct.price;
            }
        }

        if (expiryField && Array.isArray(data.productExpiryDates)) {
            const expiryProduct = data.productExpiryDates.find(item => item.code === product.code);
            if (expiryProduct) {
                expiryField.value = expiryProduct.expiryDate;
            }
        }

    }
}

function updateData() {

    fetch("/data")
        .then(response => response.json())
        .then(data => {

            document.getElementById("temperature").textContent = data.temperature.toFixed(1);
            document.getElementById("humidity").textContent = data.humidity.toFixed(1);

            const fan = document.getElementById("fan");
            const wind = document.getElementById("wind");
            const fanStatus = document.getElementById("fanStatus");

            if (data.fanState == 1) {
                fan.classList.add("fan-running");
                wind.classList.add("wind-active");
                fanStatus.textContent = "Ventilator PORNIT";
                fanStatus.classList.remove("fan-off");
                fanStatus.classList.add("fan-on");
            } else {
                fan.classList.remove("fan-running");
                wind.classList.remove("wind-active");
                fanStatus.textContent = "Ventilator OPRIT";
                fanStatus.classList.remove("fan-on");
                fanStatus.classList.add("fan-off");
            }

            updateProductConfig(data);

            document.getElementById("updateTime").textContent = new Date().toLocaleTimeString();
        })
        .catch(error => {
            console.log("Eroare:", error);
        });
}

function saveFanThreshold() {
    const thresholdInput = document.getElementById("fanTemperatureThreshold");
    if (!thresholdInput) {
        return;
    }

    const payload = new URLSearchParams();
    payload.append("fanTemperatureThreshold", thresholdInput.value);

    fetch("/setFanThreshold", {
        method: "POST",
        headers: {
            "Content-Type": "application/x-www-form-urlencoded"
        },
        body: payload.toString()
    })
    .then(response => response.json())
    .then(result => {
        console.log("Fan threshold updated:", result);
        updateData();
    })
    .catch(error => {
        console.log("Fan threshold save error:", error);
    });
}

document.getElementById("saveFanThreshold").addEventListener("click", saveFanThreshold);

setInterval(updateData, 2000);
updateData();
</script>

</body>
</html>

)rawliteral";


String extractJsonField(const String& text, const String& fieldName)
{
    int fieldPos = text.indexOf("\"" + fieldName + "\"");
    if (fieldPos < 0) {
        return "";
    }

    int colonPos = text.indexOf(':', fieldPos);
    int valueStart = text.indexOf('"', colonPos);
    if (valueStart < 0) {
        return "";
    }

    int valueEnd = text.indexOf('"', valueStart + 1);
    if (valueEnd < 0) {
        return "";
    }

    return text.substring(valueStart + 1, valueEnd);
}

int extractJsonIntField(const String& text, const String& fieldName)
{
    int fieldPos = text.indexOf("\"" + fieldName + "\"");
    if (fieldPos < 0) {
        return 0;
    }

    int colonPos = text.indexOf(':', fieldPos);
    int valueEnd = text.indexOf(',', colonPos);
    if (valueEnd < 0) {
        valueEnd = text.indexOf('}', colonPos);
    }
    if (valueEnd < 0) {
        return 0;
    }

    String value = text.substring(colonPos + 1, valueEnd);
    value.trim();
    return value.toInt();
}

float extractJsonFloatField(const String& text, const String& fieldName)
{
    int fieldPos = text.indexOf("\"" + fieldName + "\"");
    if (fieldPos < 0) {
        return 0.0f;
    }

    int colonPos = text.indexOf(':', fieldPos);
    int valueEnd = text.indexOf(',', colonPos);
    if (valueEnd < 0) {
        valueEnd = text.indexOf('}', colonPos);
    }
    if (valueEnd < 0) {
        return 0.0f;
    }

    String value = text.substring(colonPos + 1, valueEnd);
    value.trim();
    return value.toFloat();
}

bool saveFanThresholdToJson()
{
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS begin failed for fan threshold file");
        return false;
    }

    File file = SPIFFS.open(FAN_THRESHOLD_FILE, FILE_WRITE);
    if (!file) {
        Serial.println("Unable to open fan threshold file for write");
        return false;
    }

    file.print("{\"fanTemperatureThreshold\":");
    file.print(fanTemperatureThreshold, 1);
    file.print("}");
    file.close();
    return true;
}

bool loadFanThresholdFromJson()
{
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS begin failed for fan threshold file");
        return false;
    }

    if (!SPIFFS.exists(FAN_THRESHOLD_FILE)) {
        Serial.println("Fan threshold file missing, creating default fan threshold");
        return saveFanThresholdToJson();
    }

    File file = SPIFFS.open(FAN_THRESHOLD_FILE, FILE_READ);
    if (!file) {
        Serial.println("Unable to open fan threshold file");
        return false;
    }

    String payload = file.readString();
    file.close();

    int start = payload.indexOf("\"fanTemperatureThreshold\"");
    if (start >= 0) {
        int colon = payload.indexOf(':', start);
        int end = payload.indexOf('}', colon);
        if (end < 0) {
            end = payload.length();
        }

        String value = payload.substring(colon + 1, end);
        value.trim();
        fanTemperatureThreshold = value.toFloat();
    }

    return true;
}

void syncFanThresholdToArduino()
{
    Serial2.print("FAN_THRESHOLD:");
    Serial2.println(fanTemperatureThreshold, 1);
}

bool saveInventoryToJson()
{
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS begin failed");
        return false;
    }

    File file = SPIFFS.open(INVENTORY_FILE, FILE_WRITE);
    if (!file) {
        Serial.println("Unable to open inventory file for write");
        return false;
    }

    file.print("{\"products\":[");
    for (int i = 0; i < INVENTORY_PRODUCT_COUNT; i++) {
        if (i > 0) {
            file.print(",");
        }

        file.print("{\"code\":\"");
        file.print(productCodes[i]);
        file.print("\",\"stock\":");
        file.print(productStock[i]);
        file.print(",\"price\":");
        file.print(productPrices[i], 2);
        file.print(",\"expiryDate\":\"");
        file.print(productExpiryDates[i]);
        file.print("\"}");
    }
    file.print("]}");
    file.close();
    return true;
}

bool loadInventoryFromJson()
{
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS begin failed");
        return false;
    }

    if (!SPIFFS.exists(INVENTORY_FILE)) {
        Serial.println("Inventory file missing, creating default one");
        return saveInventoryToJson();
    }

    File file = SPIFFS.open(INVENTORY_FILE, FILE_READ);
    if (!file) {
        Serial.println("Unable to open inventory file");
        return false;
    }

    String payload = file.readString();
    file.close();

    int arrayStart = payload.indexOf("\"products\"");
    if (arrayStart < 0) {
        return false;
    }

    arrayStart = payload.indexOf('[', arrayStart);
    int arrayEnd = payload.lastIndexOf(']');
    String productList = payload.substring(arrayStart + 1, arrayEnd);

    int cursor = 0;
    for (int i = 0; i < INVENTORY_PRODUCT_COUNT; i++) {
        int objStart = productList.indexOf('{', cursor);
        int objEnd = productList.indexOf('}', objStart);
        if (objStart < 0 || objEnd < 0) {
            break;
        }

        String item = productList.substring(objStart, objEnd + 1);
        String code = extractJsonField(item, "code");
        int stock = extractJsonIntField(item, "stock");
        float price = extractJsonFloatField(item, "price");
        String expiryDate = extractJsonField(item, "expiryDate");

        if (code.length() > 0) {
            productCodes[i] = code;
            productStock[i] = stock;
            productPrices[i] = price;
            productExpiryDates[i] = expiryDate.length() > 0 ? expiryDate : "2026-01-01";
        }

        cursor = objEnd + 1;
    }

    return true;
}

void syncInventoryToArduino()
{
    for (int i = 0; i < INVENTORY_PRODUCT_COUNT; i++) {
        Serial2.print("STOCK:");
        Serial2.print(productCodes[i]);
        Serial2.print(":");
        Serial2.println(productStock[i]);
    }
}

bool dateInPeriod(String date)
{
    if (periodStart.length() == 0 || periodEnd.length() == 0) {
        return true;
    }

    if (date.length() < 10) {
        return false;
    }

    String datePart = date.substring(0, 10);
    return datePart >= periodStart && datePart <= periodEnd;
}

void registerSale(String code)
{
    int idx = -1;
    for (int i = 0; i < INVENTORY_PRODUCT_COUNT; i++) {
        if (productCodes[i] == code) {
            idx = i;
            break;
        }
    }

    if (idx < 0 || salesCount >= MAX_SALES_RECORDS) {
        return;
    }

    salesCodes[salesCount] = code;
    salesPrices[salesCount] = productPrices[idx];
    salesDates[salesCount] = String("2026-09-09");

    totalRevenue += productPrices[idx];
    salesCount++;

    saveSalesToJson();
}

bool saveSalesToJson()
{
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS begin failed for sales file");
        return false;
    }

    File file = SPIFFS.open(SALES_FILE, FILE_WRITE);
    if (!file) {
        Serial.println("Unable to open sales file for write");
        return false;
    }

    file.print("{\"sales\":[");
    for (int i = 0; i < salesCount; i++) {
        if (i > 0) {
            file.print(",");
        }

        file.print("{\"code\":\"");
        file.print(salesCodes[i]);
        file.print("\",\"price\":");
        file.print(salesPrices[i], 2);
        file.print(",\"date\":\"");
        file.print(salesDates[i]);
        file.print("\"}");
    }
    file.print("],\"totalRevenue\":");
    file.print(totalRevenue, 2);
    file.print(",\"periodStart\":\"");
    file.print(periodStart);
    file.print("\",\"periodEnd\":\"");
    file.print(periodEnd);
    file.print("\"}");

    file.close();
    return true;
}

bool loadSalesFromJson()
{
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS begin failed for sales file");
        return false;
    }

    if (!SPIFFS.exists(SALES_FILE)) {
        Serial.println("Sales file missing, creating default sales data");
        return saveSalesToJson();
    }

    File file = SPIFFS.open(SALES_FILE, FILE_READ);
    if (!file) {
        Serial.println("Unable to open sales file");
        return false;
    }

    String payload = file.readString();
    file.close();

    int salesArrayStart = payload.indexOf("\"sales\"");
    if (salesArrayStart < 0) {
        return false;
    }

    int arrayStart = payload.indexOf('[', salesArrayStart);
    int arrayEnd = payload.lastIndexOf(']');
    String saleList = payload.substring(arrayStart + 1, arrayEnd);

    salesCount = 0;
    totalRevenue = 0.0f;

    int cursor = 0;
    while (cursor < saleList.length()) {
        int objStart = saleList.indexOf('{', cursor);
        int objEnd = saleList.indexOf('}', objStart);
        if (objStart < 0 || objEnd < 0 || salesCount >= MAX_SALES_RECORDS) {
            break;
        }

        String item = saleList.substring(objStart, objEnd + 1);
        String code = extractJsonField(item, "code");
        float price = extractJsonFloatField(item, "price");
        String date = extractJsonField(item, "date");

        if (code.length() > 0) {
            salesCodes[salesCount] = code;
            salesPrices[salesCount] = price;
            salesDates[salesCount] = date;
            totalRevenue += price;
            salesCount++;
        }

        cursor = objEnd + 1;
    }

    int periodStartPos = payload.indexOf("\"periodStart\"");
    int periodEndPos = payload.indexOf("\"periodEnd\"");
    if (periodStartPos >= 0) {
        periodStart = extractJsonField(payload, "periodStart");
    }
    if (periodEndPos >= 0) {
        periodEnd = extractJsonField(payload, "periodEnd");
    }

    return true;
}

void resetSalesData()
{
    salesCount = 0;
    totalRevenue = 0.0f;
    for (int i = 0; i < MAX_SALES_RECORDS; i++) {
        salesCodes[i] = "";
        salesPrices[i] = 0.0f;
        salesDates[i] = "";
    }

    periodStart = "";
    periodEnd = "";
    saveSalesToJson();
}

void handleResetSales()
{
    resetSalesData();
    server.send(200, "application/json", "{\"status\":\"reset\"}");
}

void handleSetPeriod()
{
    String start = server.arg("start");
    String end = server.arg("end");
    periodStart = start;
    periodEnd = end;

    saveSalesToJson();
    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleSerial2Data()
{
    if (!Serial2.available()) {
        return;
    }

    String data = Serial2.readStringUntil('\n');
    data.trim();

    if (data.startsWith("TEMP:")) {
        String value = data.substring(5);
        temperature = value.toFloat();
        Serial.print("Temperatura: ");
        Serial.print(temperature);
        Serial.println(" °C");
    }
    else if (data.startsWith("HUM:")) {
        String value = data.substring(4);
        humidity = value.toFloat();
        Serial.print("Umiditate: ");
        Serial.print(humidity);
        Serial.println(" %");
    }
    else if (data.startsWith("PROD:")) {
        lastProductCode = data.substring(5);
        Serial.print("Selected product: ");
        Serial.println(lastProductCode);

        registerSale(lastProductCode);
    }
    else if (data.startsWith("STOCK:")) {
        String payload = data.substring(6);
        int colonIndex = payload.indexOf(':');

        if (colonIndex >= 0) {
            String code = payload.substring(0, colonIndex);
            int quantity = payload.substring(colonIndex + 1).toInt();

            for (int i = 0; i < 4; i++) {
                if (productCodes[i] == code) {
                    productStock[i] = quantity;
                    break;
                }
            }

            Serial.print("Updated stock for product ");
            Serial.print(code);
            Serial.print(": ");
            Serial.println(quantity);
        }
    }
    else if (data.startsWith("FAN:")) {
        String value = data.substring(4);
        fanState = value.toInt();
        Serial.print("Ventilator: ");

        if (fanState == 1) {
            Serial.println("PORNIT");
        } else {
            Serial.println("OPRIT");
        }
    }
}

// =====================================================
// PAGINA PRINCIPALA
// =====================================================

void handleRoot() {

    server.send(
        200,
        "text/html",
        MAIN_page
    );
}


// =====================================================
// SET PRODUCT
// =====================================================

void handleSetFanThreshold() {
    String thresholdArg = server.arg("fanTemperatureThreshold");
    if (thresholdArg.length() > 0) {
        fanTemperatureThreshold = thresholdArg.toFloat();
        saveFanThresholdToJson();
        syncFanThresholdToArduino();
    }

    String response = "{\"fanTemperatureThreshold\":";
    response += String(fanTemperatureThreshold, 1);
    response += "}";

    server.send(200, "application/json", response);
}

void handleSetProduct() {

    String code =
        server.arg("code");

    String quantityArg =
        server.arg("quantity");

    String priceArg =
        server.arg("price");

    String expiryDateArg =
        server.arg("expiryDate");

    int index = -1;

    for (
        int i = 0;
        i < 4;
        i++
    ) {

        if (
            productCodes[i] == code
        ) {

            index = i;

            break;

        }

    }

    if (
        index >= 0
    ) {

        productStock[index] =
            quantityArg.toInt();

        productPrices[index] =
            priceArg.toFloat();

        if (expiryDateArg.length() > 0) {
            productExpiryDates[index] = expiryDateArg;
        }

        // persist inventory into JSON file first
        saveInventoryToJson();

        // Send stock quantity from web UI to Arduino
        syncInventoryToArduino();

        String response = "{";

        response += "\"code\":\"";
        response += code;
        response += "\",";

        response += "\"quantity\":";
        response += String(productStock[index]);
        response += ",";

        response += "\"price\":";
        response += String(productPrices[index], 2);
        response += ",";

        response += "\"expiryDate\":\"";
        response += productExpiryDates[index];
        response += "\"}";

        server.send(
            200,
            "application/json",
            response
        );

    } else {

        server.send(
            400,
            "application/json",
            "{\"error\":\"product_not_found\"}"
        );

    }
}


// =====================================================
// DATE JSON
// =====================================================

void handleData() {

    String json = "{";


    // Temperatura

    json += "\"temperature\":";

    json += String(
        temperature,
        1
    );


    json += ",";


    // Umiditate

    json += "\"humidity\":";

    json += String(
        humidity,
        1
    );


    json += ",";


    // Ventilator

    json += "\"fanState\":";

    json += String(
        fanState
    );


    json += ",";


    // Selected product code after a successful transaction

    json += "\"productCode\":\"";

    json += lastProductCode;

    json += "\"";


    json += ",";


    // Stock per column / product

    json += "\"productStock\":[";

    for (
        int i = 0;
        i < 4;
        i++
    ) {

        if (
            i > 0
        ) {

            json += ",";

        }

        json += "{\"code\":\"";

        json += productCodes[i];

        json += "\",\"quantity\":";

        json += String(
            productStock[i]
        );

        json += "}";

    }

    json += "]";


    json += ",";


    // Product prices

    json += "\"productPrices\":[";

    for (
        int i = 0;
        i < 4;
        i++
    ) {

        if (
            i > 0
        ) {

            json += ",";

        }

        json += "{\"code\":\"";

        json += productCodes[i];

        json += "\",\"price\":";

        json += String(
            productPrices[i],
            2
        );

        json += "}";

    }

    json += "]";


    json += ",";


    // Expiry dates per product

    json += "\"productExpiryDates\":[";

    for (
        int i = 0;
        i < 4;
        i++
    ) {

        if (
            i > 0
        ) {

            json += ",";

        }

        json += "{\"code\":\"";

        json += productCodes[i];

        json += "\",\"expiryDate\":\"";

        json += productExpiryDates[i];

        json += "\"}";

    }

    json += "]";


    json += ",\"fanTemperatureThreshold\":";
    json += String(fanTemperatureThreshold, 1);

    json += ",";


    // Dashboard sales revenue information

    float filteredRevenue = 0.0f;

    json += "\"totalRevenue\":";
    json += String(totalRevenue, 2);

    json += ",\"periodStart\":\"";
    json += periodStart;
    json += "\",";

    json += "\"periodEnd\":\"";
    json += periodEnd;
    json += "\",";

    json += "\"sales\":[";

    for (int i = 0; i < salesCount; i++) {

        if (!dateInPeriod(salesDates[i])) {
            continue;
        }

        if (i > 0) {
            json += ",";
        }

        filteredRevenue += salesPrices[i];

        json += "{\"code\":\"";
        json += salesCodes[i];
        json += "\",\"price\":";
        json += String(salesPrices[i], 2);
        json += ",\"date\":\"";
        json += salesDates[i];
        json += "\"}";
    }

    json += "]";

    json += ",\"filteredRevenue\":";
    json += String(filteredRevenue, 2);

    json += "}";


    server.send(
        200,
        "application/json",
        json
    );
}


// =====================================================
// SETUP
// =====================================================

void setup() {


    // ================================================
    // Serial Monitor
    // ================================================

    Serial.begin(
        115200
    );


    // ================================================
    // UART Mega <-> ESP32
    // ================================================

    Serial2.begin(
        9600,
        SERIAL_8N1,
        RX_PIN,
        TX_PIN
    );

    // ================================================
    // SPIFFS inventory
    // ================================================

    SPIFFS.begin(true);
    loadInventoryFromJson();
    loadFanThresholdFromJson();
    loadSalesFromJson();
    syncInventoryToArduino();
    syncFanThresholdToArduino();


    // ================================================
    // WiFi
    // ================================================

    Serial.println();

    Serial.println(
        "Conectare la WiFi..."
    );

    Serial.print(
        "SSID: "
    );

    Serial.println(
        ssid
    );


    WiFi.begin(
        ssid,
        password
    );


    while (
        WiFi.status() != WL_CONNECTED
    ) {

        delay(500);

        Serial.print(".");
    }


    Serial.println();

    Serial.println(
        "WiFi conectat!"
    );


    Serial.print(
        "IP ESP32: "
    );

    Serial.println(
        WiFi.localIP()
    );


    // ================================================
    // Web Server
    // ================================================

    server.on(
        "/",
        handleRoot
    );


    server.on(
        "/data",
        handleData
    );


    server.on(
        "/setProduct",
        HTTP_POST,
        handleSetProduct
    );

    server.on(
        "/setFanThreshold",
        HTTP_POST,
        handleSetFanThreshold
    );

    server.on(
        "/resetSales",
        HTTP_POST,
        handleResetSales
    );

    server.on(
        "/setPeriod",
        HTTP_GET,
        handleSetPeriod
    );

    server.begin();


    Serial.println(
        "Web server pornit!"
    );

}


// =====================================================
// LOOP
// =====================================================

void loop() {
    static unsigned long lastSerialTask = 0;
    static unsigned long lastServerTask = 0;

    // Handle the HTTP layer in small service slices, but allow the UI
    // and the logging instructions to remain independent from the task budget.
    if (millis() - lastServerTask >= MAX_EXECUTION_SLICE_MS) {
        server.handleClient();
        lastServerTask = millis();
    }

    // Keep the Arduino serial event parser in a bounded 50 ms task window.
    if (millis() - lastSerialTask >= MAX_EXECUTION_SLICE_MS) {
        handleSerial2Data();
        lastSerialTask = millis();
    }

    // WiFi recovery is allowed to reconnect without any long blocking delay.
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi deconectat!");
        WiFi.disconnect();
        WiFi.begin(ssid, password);
    }
}