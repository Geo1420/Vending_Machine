function getDemoData() {
    return {
        temperature: 24.7,
        humidity: 45.3,
        fanState: 1,
        productCode: "11",
        productStock: [
            { code: "11", quantity: 5 },
            { code: "12", quantity: 3 },
            { code: "13", quantity: 4 },
            { code: "14", quantity: 2 }
        ],
        productPrices: [
            { code: "11", price: 10.00 },
            { code: "12", price: 12.50 },
            { code: "13", price: 8.00 },
            { code: "14", price: 9.00 }
        ],
        productExpiryDates: [
            { code: "11", expiryDate: "2026-09-15" },
            { code: "12", expiryDate: "2026-10-15" },
            { code: "13", expiryDate: "2026-11-15" },
            { code: "14", expiryDate: "2026-12-15" }
        ],
        fanTemperatureThreshold: 28.0,
        totalRevenue: 126.50,
        periodStart: "2026-09-01",
        periodEnd: "2026-09-30",
        sales: [
            { code: "11", price: 10.00, date: "2026-09-09" },
            { code: "12", price: 12.50, date: "2026-09-09" },
            { code: "13", price: 8.00, date: "2026-09-10" },
            { code: "14", price: 9.00, date: "2026-09-10" }
        ]
    };
}

function setStaticDemoValues() {
    document.getElementById("temperature").textContent = "24.7";
    document.getElementById("humidity").textContent = "45.3";
    document.getElementById("fanStatus").textContent = "Ventilator PORNIT";
    document.getElementById("fanStatus").className = "fan-status fan-on";
    document.getElementById("fan").className = "fan fan-running";
    document.getElementById("wind").className = "wind wind-active";

    document.getElementById("totalRevenue").textContent = "126.50 RON";
    document.getElementById("salesCount").textContent = "8";
    document.getElementById("periodRange").textContent = "01.09.2026 - 30.09.2026";
    document.getElementById("periodStart").value = "2026-09-01";
    document.getElementById("periodEnd").value = "2026-09-30";

    const rows = [
        ["11", "5", "10.00", "2026-09-15"],
        ["12", "3", "12.50", "2026-10-15"],
        ["13", "4", "8.00", "2026-11-15"],
        ["14", "2", "9.00", "2026-12-15"]
    ];

    for (const [code, stock, price, date] of rows) {
        document.getElementById("stock-" + code).value = stock;
        document.getElementById("price-" + code).value = price;
        document.getElementById("expiry-" + code).value = date;
    }

    document.getElementById("fanTemperatureThreshold").value = "28.0";
}

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
        const demoData = getDemoData();
        const data = demoData;
        // keep UI in demo mode and show locally updated values
        updateProductConfig(data);
        updateDashboard(data);
    });
}

function resetSalesData() {

    fetch("/resetSales", {
        method: "POST"
    })
    .then(response => response.json())
    .then(result => {
        console.log("Sales reset:", result);
        updateData();
    })
    .catch(error => {
        console.log("Reset error:", error);
        const demoData = getDemoData();
        demoData.sales = [];
        demoData.totalRevenue = 0.0;
        updateDashboard(demoData);
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
        const demoData = getDemoData();
        demoData.fanTemperatureThreshold = Number(thresholdInput.value);
        updateProductConfig(demoData);
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

    if (data.fanTemperatureThreshold !== undefined) {
        const thresholdInput = document.getElementById("fanTemperatureThreshold");
        if (thresholdInput) {
            thresholdInput.value = data.fanTemperatureThreshold;
        }
    }
}

function updateDashboard(data) {

    if (data.totalRevenue !== undefined) {
        document.getElementById("totalRevenue").textContent = Number(data.totalRevenue).toFixed(2) + " RON";
    }

    if (data.periodStart) {
        document.getElementById("periodStart").value = data.periodStart;
    }

    if (data.periodEnd) {
        document.getElementById("periodEnd").value = data.periodEnd;
    }

    if (data.periodStart && data.periodEnd) {
        document.getElementById("periodRange").textContent = data.periodStart + " - " + data.periodEnd;
    }

    const salesCount = Array.isArray(data.sales) ? data.sales.length : 0;
    document.getElementById("salesCount").textContent = salesCount;

    const salesTableBody = document.getElementById("salesTableBody");
    if (salesTableBody) {
        salesTableBody.innerHTML = "";

        if (Array.isArray(data.sales)) {
            for (let i = 0; i < data.sales.length; i++) {
                const sale = data.sales[i];
                const row = document.createElement("tr");
                row.innerHTML = "<td>" + sale.code + "</td><td>" + Number(sale.price).toFixed(2) + " RON</td><td>" + sale.date + "</td>";
                salesTableBody.appendChild(row);
            }
        }
    }
}

function updateData() {

    fetch("/data")
        .then(function(response) {
            if (!response.ok) {
                throw new Error("ESP32 unavailable or not connected");
            }
            return response.json();
        })
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
            updateDashboard(data);

            document.getElementById("updateTime").textContent = new Date().toLocaleTimeString();
        })
        .catch(error => {
            console.log("Eroare, folosesc date demo:", error);
            const data = getDemoData();
            document.getElementById("temperature").textContent = data.temperature.toFixed(1);
            document.getElementById("humidity").textContent = data.humidity.toFixed(1);

            const fan = document.getElementById("fan");
            const wind = document.getElementById("wind");
            const fanStatus = document.getElementById("fanStatus");
            fan.className = "fan fan-running";
            wind.className = "wind wind-active";
            fanStatus.textContent = "Ventilator PORNIT";
            fanStatus.className = "fan-status fan-on";

            updateProductConfig(data);
            updateDashboard(data);
            document.getElementById("updateTime").textContent = new Date().toLocaleTimeString();
        });
}

document.getElementById("resetSales").addEventListener("click", resetSalesData);
document.getElementById("applyPeriod").addEventListener("click", function() {
    const periodStart = document.getElementById("periodStart").value;
    const periodEnd = document.getElementById("periodEnd").value;

    fetch("/setPeriod?start=" + encodeURIComponent(periodStart) + "&end=" + encodeURIComponent(periodEnd), {
        method: "GET"
    })
    .then(response => response.json())
    .then(result => {
        updateData();
    })
    .catch(error => {
        console.log("Period error:", error);
        const demoData = getDemoData();
        demoData.periodStart = periodStart;
        demoData.periodEnd = periodEnd;
        updateDashboard(demoData);
    });
});

document.getElementById("saveFanThreshold").addEventListener("click", saveFanThreshold);

setInterval(updateData, 2000);
updateData();
