async function loadFlightOptions() {
    try {
        const flights = await fetchFlights();
        const select = document.getElementById('flight-select');
        select.innerHTML = '<option value="">— 請選擇航班 —</option>';

        flights.forEach(f => {
            const opt = document.createElement('option');
            opt.value = f.id;
            opt.textContent = `${f.flight_number}  ·  ${f.origin} → ${f.destination}  ·  ${formatPrice(f.current_price)}`;
            select.appendChild(opt);
        });
    } catch (err) {
        showToast(err.message, 'danger');
    }
}

let priceChart = null;

async function loadAnalysis() {
    const id = document.getElementById('flight-select').value;
    const resultEl = document.getElementById('analysis-result');

    if (!id) {
        resultEl.innerHTML = `
            <div class="empty-state">
                <i class="bi bi-graph-up-arrow"></i>
                <p class="mb-0">請選擇航班以查看價格分析與趨勢圖</p>
            </div>`;
        if (priceChart) { priceChart.destroy(); priceChart = null; }
        return;
    }

    try {
        resultEl.innerHTML = '<p class="text-center text-muted py-5 loading-pulse"><i class="bi bi-hourglass-split me-2"></i>分析中...</p>';
        const data = await fetchAnalysis(id);
        renderAnalysis(data);
    } catch (err) {
        showToast(err.message, 'danger');
    }
}

function renderAlertTags(tags) {
    if (!tags || tags.length === 0) return '';
    let html = '<div class="mb-4">';
    tags.forEach(tag => {
        const cssClass = tag.type === 'special_offer' ? 'alert-banner' :
                         tag.type === 'historical_low' ? 'alert-tag alert-tag-low' :
                         'alert-tag alert-tag-drop';
        html += `
            <div class="${cssClass} mb-2">
                <strong>${tag.label}</strong>
                <span class="small ms-2 opacity-75">${tag.detail}</span>
            </div>`;
    });
    html += '</div>';
    return html;
}

function renderPriceChart(chartData) {
    const canvas = document.getElementById('price-chart');
    if (!canvas || !chartData?.labels?.length) return;

    if (priceChart) priceChart.destroy();

    priceChart = new Chart(canvas, {
        type: 'line',
        data: {
            labels: chartData.labels,
            datasets: [{
                label: '票價 (NT$)',
                data: chartData.prices,
                borderColor: '#2563eb',
                backgroundColor: 'rgba(37, 99, 235, 0.08)',
                borderWidth: 3,
                pointRadius: 6,
                pointBackgroundColor: '#2563eb',
                pointBorderColor: '#fff',
                pointBorderWidth: 2,
                tension: 0.35,
                fill: true
            }]
        },
        options: {
            responsive: true,
            plugins: {
                legend: { display: false },
                title: {
                    display: true,
                    text: '價格趨勢（資料來源：SQLite price_history）',
                    font: { family: 'Plus Jakarta Sans', size: 13, weight: '600' },
                    color: '#64748b',
                    padding: { bottom: 16 }
                }
            },
            scales: {
                x: { grid: { display: false }, ticks: { font: { size: 11 } } },
                y: {
                    grid: { color: 'rgba(0,0,0,0.05)' },
                    ticks: { callback: v => 'NT$ ' + Number(v).toLocaleString(), font: { size: 11 } }
                }
            }
        }
    });
}

function renderAnalysis(data) {
    const resultEl = document.getElementById('analysis-result');
    const f = data.flight;

    let historyHtml = '';
    if (data.history?.length > 0) {
        historyHtml = '<h6 class="fw-bold mt-4 mb-3"><i class="bi bi-clock-history me-2 text-primary"></i>價格歷史紀錄</h6><div class="row g-2">';
        data.history.forEach(h => {
            const date = h.record_time.split(' ')[0];
            historyHtml += `
                <div class="col-md-4">
                    <div class="history-item">
                        <div class="text-muted small"><i class="bi bi-calendar3 me-1"></i>${date}</div>
                        <div class="fw-bold price-tag">${formatPrice(h.price)}</div>
                    </div>
                </div>`;
        });
        historyHtml += '</div>';
    }

    resultEl.innerHTML = `
        <div class="d-flex align-items-center gap-3 mb-4 p-3 rounded-3" style="background:#f8fafc;border:1px solid #e2e8f0">
            <div class="stat-icon-wrap blue"><i class="bi bi-airplane"></i></div>
            <div>
                <span class="flight-code fs-6">${f.flight_number}</span>
                <span class="route-text ms-2">${f.origin} → ${f.destination}</span>
                <div class="text-muted small mt-1">出發日期：${f.departure_date}</div>
            </div>
        </div>
        ${renderAlertTags(data.alert_tags)}
        <div class="row g-3">
            <div class="col-md-3 col-sm-6">
                <div class="card stat-card"><div class="card-body py-3">
                    <div class="stat-icon-wrap green" style="width:40px;height:40px;font-size:1rem"><i class="bi bi-arrow-down"></i></div>
                    <div class="stat-content"><div class="stat-label">最低價</div><div class="stat-value price-low" style="font-size:1.3rem">${formatPrice(data.min_price)}</div></div>
                </div></div>
            </div>
            <div class="col-md-3 col-sm-6">
                <div class="card stat-card"><div class="card-body py-3">
                    <div class="stat-icon-wrap red" style="width:40px;height:40px;font-size:1rem"><i class="bi bi-arrow-up"></i></div>
                    <div class="stat-content"><div class="stat-label">最高價</div><div class="stat-value price-high" style="font-size:1.3rem">${formatPrice(data.max_price)}</div></div>
                </div></div>
            </div>
            <div class="col-md-3 col-sm-6">
                <div class="card stat-card"><div class="card-body py-3">
                    <div class="stat-icon-wrap cyan" style="width:40px;height:40px;font-size:1rem"><i class="bi bi-calculator"></i></div>
                    <div class="stat-content"><div class="stat-label">平均價</div><div class="stat-value" style="font-size:1.3rem">${formatPrice(data.average_price)}</div></div>
                </div></div>
            </div>
            <div class="col-md-3 col-sm-6">
                <div class="card stat-card"><div class="card-body py-3">
                    <div class="stat-icon-wrap orange" style="width:40px;height:40px;font-size:1rem"><i class="bi bi-percent"></i></div>
                    <div class="stat-content"><div class="stat-label">價格跌幅</div><div class="stat-value" style="font-size:1.3rem">${formatPercent(data.price_drop_percent)}</div></div>
                </div></div>
            </div>
        </div>
        <div class="chart-container mt-4">
            <canvas id="price-chart" height="100"></canvas>
        </div>
        ${historyHtml}
    `;

    renderPriceChart(data.chart);
}

document.addEventListener('DOMContentLoaded', () => {
    setActiveNav('analysis');
    loadFlightOptions();
    document.getElementById('flight-select').addEventListener('change', loadAnalysis);
});
