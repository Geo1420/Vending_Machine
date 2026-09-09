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

int fanState = 0;
String lastProductCode = "";
String productCodes[] = {"11", "12", "13", "14"};
int productStock[] = {5, 3, 4, 2};
float productPrices[] = {10.0, 12.5, 8.0, 9.0};

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

    <meta name="viewport"
          content="width=device-width, initial-scale=1.0">

    <title>Monitorizare Vending Machine</title>


    <style>

        * {
            box-sizing: border-box;
        }


        body {

            margin: 0;

            font-family: Arial, sans-serif;

            background: #f2f2f2;

            text-align: center;

        }


        /* =========================================
           HEADER
           ========================================= */

        header {

            background: #222;

            color: white;

            padding: 20px;

        }


        header h1 {

            margin: 0;

            font-size: 28px;

        }


        /* =========================================
           CONTAINER
           ========================================= */

        .container {

            display: flex;

            justify-content: center;

            align-items: stretch;

            gap: 30px;

            margin-top: 50px;

            padding: 20px;

            flex-wrap: wrap;

        }


        /* =========================================
           CARD
           ========================================= */

        .card {

            background: white;

            width: 260px;

            min-height: 220px;

            padding: 30px;

            border-radius: 15px;

            box-shadow:
                0 4px 10px rgba(0,0,0,0.15);

        }


        .title {

            font-size: 22px;

            margin-bottom: 20px;

        }


        .value {

            font-size: 48px;

            font-weight: bold;

        }


        .unit {

            font-size: 25px;

        }


        /* =========================================
           VENTILATOR
           ========================================= */

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


        /* Centrul ventilatorului */

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


        /* Palele ventilatorului */

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

            transform:
                translate(-50%, -100%)
                rotate(0deg);

        }


        .blade2 {

            transform:
                translate(-50%, -100%)
                rotate(120deg);

        }


        .blade3 {

            transform:
                translate(-50%, -100%)
                rotate(240deg);

        }


        /* =========================================
           ANIMATIE ROTIRE
           ========================================= */

        .fan-running {

            animation:
                rotateFan 0.35s linear infinite;

        }


        @keyframes rotateFan {

            from {

                transform: rotate(0deg);

            }

            to {

                transform: rotate(360deg);

            }

        }


        /* =========================================
           VALURI DE VANT
           ========================================= */

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

            animation:
                windAnimation 1s linear infinite;

        }


        @keyframes windAnimation {

            0% {

                transform:
                    translateY(-50%)
                    translateX(20px);

                opacity: 0;

            }

            30% {

                opacity: 1;

            }

            70% {

                opacity: 1;

            }

            100% {

                transform:
                    translateY(-50%)
                    translateX(70px);

                opacity: 0;

            }

        }


        /* =========================================
           STATUS VENTILATOR
           ========================================= */

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


        /* =========================================
           STATUS GENERAL
           ========================================= */

        .status {

            margin-top: 30px;

            color: #555;

            font-size: 16px;

        }


        /* =========================================
           PRODUCT CONFIGURATION
           ========================================= */

        .config-panel {

            background: white;

            width: min(820px, calc(100vw - 40px));

            margin: 30px auto 20px auto;

            padding: 25px;

            border-radius: 15px;

            box-shadow:
                0 4px 10px rgba(0,0,0,0.15);

        }


        .config-panel .title {

            font-size: 22px;

            margin-bottom: 20px;

            color: #222;

        }


        .product-config {

            display: flex;

            flex-direction: column;

            gap: 12px;

        }


        .product-config-row {

            display: grid;

            grid-template-columns: 110px 130px 130px 120px;

            gap: 10px;

            align-items: center;

            justify-content: center;

        }


        .product-config-header {

            font-weight: bold;

            color: #444;

            font-size: 14px;

            text-align: center;

        }


        .product-code {

            text-align: center;

            font-weight: bold;

            font-size: 21px;

            color: #222;

        }


        .product-config input {

            width: 110px;

            padding: 8px;

            border-radius: 8px;

            border: 1px solid #bbb;

            text-align: center;

        }


        .product-config button {

            padding: 10px 14px;

            border-radius: 8px;

            border: none;

            background: #2e7d32;

            color: white;

            cursor: pointer;

        }


        .product-config button:hover {

            background: #256b29;

        }

    </style>

</head>


<body>


<!-- =================================================
     HEADER
     ================================================= -->

<header>

    <h1>
        Monitorizare Vending Machine
    </h1>

</header>


<!-- =================================================
     CARDS
     ================================================= -->

<div class="container">


    <!-- =============================================
         TEMPERATURA
         ============================================= -->

    <div class="card">

        <div class="title">
            🌡️ Temperatura
        </div>

        <div class="value">

            <span id="temperature">
                --
            </span>

            <span class="unit">
                °C
            </span>

        </div>

    </div>


    <!-- =============================================
         UMIDITATE
         ============================================= -->

    <div class="card">

        <div class="title">
            💧 Umiditate
        </div>

        <div class="value">

            <span id="humidity">
                --
            </span>

            <span class="unit">
                %
            </span>

        </div>

    </div>


    <!-- =============================================
         VENTILATOR
         ============================================= -->

    <div class="card">

        <div class="title">
            🌀 Ventilator
        </div>


        <div class="fan-container">


            <div id="fan"
                 class="fan">


                <div class="blade blade1"></div>

                <div class="blade blade2"></div>

                <div class="blade blade3"></div>


                <div class="fan-center"></div>

            </div>


            <!-- Valuri de vant -->

            <div id="wind"
                 class="wind">

                ))) 

            </div>

        </div>


        <div id="fanStatus"
             class="fan-status fan-off">

            Ventilator OPRIT

        </div>

    </div>


</div>


<!-- =================================================
     PRODUCT CONFIGURATION
     ================================================= -->

<div class="config-panel">

    <div class="title">
        🧃 Product configuration
    </div>

    <div class="product-config">

        <div class="product-config-row">
            <div class="product-config-header">
                Cod
            </div>
            <div class="product-config-header">
                Cantitate
            </div>
            <div class="product-config-header">
                Pret
            </div>
            <div class="product-config-header">
                Acțiune
            </div>
        </div>

        <div class="product-config-row">
            <div class="product-code">
                11
            </div>
            <input id="stock-11" type="number" min="0" value="5">
            <input id="price-11" type="number" min="0" step="0.01" value="10.00">
            <button onclick="saveProduct('11')">
                Salveaza
            </button>
        </div>

        <div class="product-config-row">
            <div class="product-code">
                12
            </div>
            <input id="stock-12" type="number" min="0" value="3">
            <input id="price-12" type="number" min="0" step="0.01" value="12.50">
            <button onclick="saveProduct('12')">
                Salveaza
            </button>
        </div>

        <div class="product-config-row">
            <div class="product-code">
                13
            </div>
            <input id="stock-13" type="number" min="0" value="4">
            <input id="price-13" type="number" min="0" step="0.01" value="8.00">
            <button onclick="saveProduct('13')">
                Salveaza
            </button>
        </div>

        <div class="product-config-row">
            <div class="product-code">
                14
            </div>
            <input id="stock-14" type="number" min="0" value="2">
            <input id="price-14" type="number" min="0" step="0.01" value="9.00">
            <button onclick="saveProduct('14')">
                Salveaza
            </button>
        </div>

    </div>

</div>


<!-- =================================================
     ULTIMA ACTUALIZARE
     ================================================= -->

<div class="status">

    Ultima actualizare:

    <span id="updateTime">
        --
    </span>

</div>


<!-- =================================================
     JAVASCRIPT
     ================================================= -->

<script>

function saveProduct(code) {

    const stockInput = document.getElementById("stock-" + code);
    const priceInput = document.getElementById("price-" + code);

    const payload = new URLSearchParams();
    payload.append("code", code);
    payload.append("quantity", stockInput.value);
    payload.append("price", priceInput.value);

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

        if (stockField) {
            stockField.value = product.quantity;
        }

        if (priceField && Array.isArray(data.productPrices)) {
            const priceProduct = data.productPrices.find(item => item.code === product.code);
            if (priceProduct) {
                priceField.value = priceProduct.price;
            }
        }

    }
}

function updateData() {


    fetch("/data")


        .then(response => response.json())


        .then(data => {


            // =====================================
            // TEMPERATURA
            // =====================================

            document.getElementById(
                "temperature"
            ).textContent =
                data.temperature.toFixed(1);


            // =====================================
            // UMIDITATE
            // =====================================

            document.getElementById(
                "humidity"
            ).textContent =
                data.humidity.toFixed(1);


            // =====================================
            // VENTILATOR
            // =====================================

            const fan =
                document.getElementById("fan");

            const wind =
                document.getElementById("wind");

            const fanStatus =
                document.getElementById("fanStatus");


            if (data.fanState == 1) {


                // Pornim ventilatorul

                fan.classList.add(
                    "fan-running"
                );


                // Pornim valurile

                wind.classList.add(
                    "wind-active"
                );


                // Schimbam textul

                fanStatus.textContent =
                    "Ventilator PORNIT";


                fanStatus.classList.remove(
                    "fan-off"
                );


                fanStatus.classList.add(
                    "fan-on"
                );


            } else {


                // Oprim ventilatorul

                fan.classList.remove(
                    "fan-running"
                );


                // Oprim valurile

                wind.classList.remove(
                    "wind-active"
                );


                // Schimbam textul

                fanStatus.textContent =
                    "Ventilator OPRIT";


                fanStatus.classList.remove(
                    "fan-on"
                );


                fanStatus.classList.add(
                    "fan-off"
                );

            }


            // =====================================
            // PRODUCT CONFIGURATION
            // =====================================

            updateProductConfig(data);


            // =====================================
            // UPDATE TIME
            // =====================================

            document.getElementById(
                "updateTime"
            ).textContent =
                new Date().toLocaleTimeString();


        })


        .catch(error => {

            console.log(
                "Eroare:",
                error
            );

        });

}


// Actualizare date la fiecare 2 secunde

setInterval(
    updateData,
    2000
);


// Prima citire

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
        file.print("}");
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

        if (code.length() > 0) {
            productCodes[i] = code;
            productStock[i] = stock;
            productPrices[i] = price;
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

void handleSetProduct() {

    String code =
        server.arg("code");

    String quantityArg =
        server.arg("quantity");

    String priceArg =
        server.arg("price");

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
        response += "}";

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
    syncInventoryToArduino();


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