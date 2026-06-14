async function loadStatistics() {
    try {
        const stats = await fetchStats();

        document.getElementById('stat-total').textContent = stats.total_flights;
        document.getElementById('stat-avg').textContent = formatPrice(stats.average_price);
        document.getElementById('stat-min').textContent = formatPrice(stats.min_price);
        document.getElementById('stat-max').textContent = formatPrice(stats.max_price);
        document.getElementById('stat-discount').textContent = stats.discount_count;
        document.getElementById('stat-historical-low').textContent = stats.historical_low_count ?? 0;
        document.getElementById('stat-consecutive-drop').textContent = stats.consecutive_drop_count ?? 0;

        renderDiscountTable(stats.discounts);
        renderAdvancedAlertsTable(stats.advanced_alerts);
        renderAllFlightsRanking(stats.ranking);
    } catch (err) {
        showToast(err.message, 'danger');
    }
}

function renderDiscountTable(discounts) {
    const tbody = document.getElementById('discount-body');
    tbody.innerHTML = '';

    if (!discounts?.length) {
        tbody.innerHTML = '<tr><td colspan="5" class="text-center text-muted py-4">目前沒有優惠航班</td></tr>';
        return;
    }

    discounts.forEach(d => {
        const tr = document.createElement('tr');
        tr.innerHTML = `
            <td><span class="flight-code">${d.flight_number}</span></td>
            <td><span class="route-text">${d.origin} → ${d.destination}</span></td>
            <td><span class="price-tag low">${formatPrice(d.current_price)}</span></td>
            <td class="price-low">${formatPercent(d.discount_percent)}</td>
            <td><span class="discount-badge">${d.alert}</span></td>
        `;
        tbody.appendChild(tr);
    });
}

function renderAdvancedAlertsTable(alerts) {
    const tbody = document.getElementById('advanced-body');
    if (!tbody) return;

    tbody.innerHTML = '';
    if (!alerts?.length) {
        tbody.innerHTML = '<tr><td colspan="4" class="text-center text-muted py-4">目前沒有進階警報</td></tr>';
        return;
    }

    alerts.forEach(a => {
        const tags = (a.tags || []).map(t =>
            `<span class="insight-badge d-inline-block mb-1 me-1">${t.label}</span>`
        ).join('');
        const tr = document.createElement('tr');
        tr.innerHTML = `
            <td><span class="flight-code">${a.flight_number}</span></td>
            <td><span class="route-text">${a.origin} → ${a.destination}</span></td>
            <td><span class="price-tag">${formatPrice(a.current_price)}</span></td>
            <td>${tags}</td>
        `;
        tbody.appendChild(tr);
    });
}

function renderAllFlightsRanking(ranking) {
    const tbody = document.getElementById('all-ranking-body');
    tbody.innerHTML = '';

    if (!ranking?.length) {
        tbody.innerHTML = '<tr><td colspan="5" class="text-center text-muted py-4">尚無資料</td></tr>';
        return;
    }

    ranking.forEach((f, i) => {
        const tr = document.createElement('tr');
        tr.innerHTML = `
            <td><span class="rank-badge ${i < 3 ? 'top' : ''}">${i + 1}</span></td>
            <td><span class="flight-code">${f.flight_number}</span></td>
            <td><span class="route-text">${f.origin} → ${f.destination}</span></td>
            <td>${f.departure_date}</td>
            <td><span class="price-tag ${i === 0 ? 'low' : ''}">${formatPrice(f.current_price)}</span></td>
        `;
        tbody.appendChild(tr);
    });
}

document.addEventListener('DOMContentLoaded', () => {
    setActiveNav('statistics');
    loadStatistics();
});
