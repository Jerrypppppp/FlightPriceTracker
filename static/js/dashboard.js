async function loadDashboard() {
    try {
        const stats = await fetchStats();

        document.getElementById('total-flights').textContent = stats.total_flights;
        document.getElementById('avg-price').textContent = formatPrice(stats.average_price);
        document.getElementById('min-price').textContent = formatPrice(stats.min_price);

        const cheapestEl = document.getElementById('cheapest-flight');
        if (stats.cheapest_flight) {
            const c = stats.cheapest_flight;
            cheapestEl.innerHTML = `<span class="flight-code">${c.flight_number}</span> <span class="route-text">${c.origin}→${c.destination}</span>`;
        } else {
            cheapestEl.textContent = '—';
        }

        document.getElementById('discount-count').textContent = stats.discount_count;
        const badge = document.getElementById('discount-badge');
        if (badge) badge.textContent = stats.discount_count;
        document.getElementById('historical-low-count').textContent = stats.historical_low_count ?? 0;
        document.getElementById('consecutive-drop-count').textContent = stats.consecutive_drop_count ?? 0;

        renderDiscountAlerts(stats.discounts);
        renderAdvancedAlerts(stats.advanced_alerts);
        renderRanking(stats.ranking);
    } catch (err) {
        console.error(err);
    }
}

function renderDiscountAlerts(discounts) {
    const container = document.getElementById('discount-alerts');
    container.innerHTML = '';

    if (!discounts || discounts.length === 0) {
        container.innerHTML = '<div class="empty-state py-3"><i class="bi bi-check-circle"></i><p class="mb-0 small">目前沒有特價航班</p></div>';
        return;
    }

    discounts.forEach(d => {
        const div = document.createElement('div');
        div.className = 'alert-banner mb-3';
        div.innerHTML = `
            ${d.alert}<br>
            <span class="flight-code bg-white bg-opacity-25">${d.flight_number}</span>
            &nbsp;${d.origin} → ${d.destination}<br>
            <span class="small opacity-90 mt-1 d-inline-block">
                目前 ${formatPrice(d.current_price)} · 平均 ${formatPrice(d.average_price)} · 省 ${formatPercent(d.discount_percent)}
            </span>
        `;
        container.appendChild(div);
    });
}

function renderAdvancedAlerts(alerts) {
    const container = document.getElementById('advanced-alerts');
    if (!container) return;

    container.innerHTML = '';
    if (!alerts || alerts.length === 0) {
        container.innerHTML = '<div class="empty-state py-3"><i class="bi bi-shield-check"></i><p class="mb-0 small">目前沒有進階警報</p></div>';
        return;
    }

    alerts.slice(0, 5).forEach(a => {
        const tags = (a.tags || []).map(t => `<span class="insight-badge me-1">${t.label}</span>`).join('');
        const div = document.createElement('div');
        div.className = 'insight-item';
        div.innerHTML = `
            <div class="d-flex justify-content-between align-items-start">
                <div>
                    <span class="flight-code">${a.flight_number}</span>
                    <span class="route-text ms-2">${a.origin} → ${a.destination}</span>
                </div>
                <span class="price-tag low">${formatPrice(a.current_price)}</span>
            </div>
            <div class="mt-2">${tags}</div>
        `;
        container.appendChild(div);
    });
}

function renderRanking(ranking) {
    const tbody = document.getElementById('ranking-body');
    tbody.innerHTML = '';

    if (!ranking || ranking.length === 0) {
        tbody.innerHTML = '<tr><td colspan="5" class="text-center text-muted py-4">尚無資料</td></tr>';
        return;
    }

    ranking.forEach((f, i) => {
        const tr = document.createElement('tr');
        tr.innerHTML = `
            <td><span class="rank-badge ${i < 3 ? 'top' : ''}">${i + 1}</span></td>
            <td><span class="flight-code">${f.flight_number}</span></td>
            <td><span class="route-text">${f.origin} <i class="bi bi-arrow-right mx-1"></i> ${f.destination}</span></td>
            <td>${f.departure_date}</td>
            <td><span class="price-tag low">${formatPrice(f.current_price)}</span></td>
        `;
        tbody.appendChild(tr);
    });
}

document.addEventListener('DOMContentLoaded', () => {
    setActiveNav('dashboard');
    loadDashboard();
});
