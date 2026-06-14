const API_BASE = '/api';

async function fetchJSON(url, options = {}) {
    const res = await fetch(url, {
        headers: { 'Content-Type': 'application/json' },
        ...options
    });
    if (!res.ok) {
        const err = await res.json().catch(() => ({ error: res.statusText }));
        throw new Error(err.error || 'Request failed');
    }
    return res.json();
}

async function fetchFlights(params = {}) {
    const qs = new URLSearchParams(params).toString();
    const url = qs ? `${API_BASE}/flights?${qs}` : `${API_BASE}/flights`;
    return fetchJSON(url);
}

async function fetchStats() {
    return fetchJSON(`${API_BASE}/stats`);
}

async function createFlight(data) {
    return fetchJSON(`${API_BASE}/flights`, {
        method: 'POST',
        body: JSON.stringify(data)
    });
}

async function updateFlight(id, data) {
    return fetchJSON(`${API_BASE}/flights/${id}`, {
        method: 'PUT',
        body: JSON.stringify(data)
    });
}

async function deleteFlight(id) {
    return fetchJSON(`${API_BASE}/flights/${id}`, {
        method: 'DELETE'
    });
}

async function fetchHistory(id) {
    return fetchJSON(`${API_BASE}/flights/${id}/history`);
}

async function fetchAnalysis(id) {
    return fetchJSON(`${API_BASE}/flights/${id}/analysis`);
}

function formatPrice(price) {
    return 'NT$ ' + Number(price).toLocaleString();
}

function formatPercent(value) {
    return Number(value).toFixed(1) + '%';
}

function setActiveNav(page) {
    document.querySelectorAll('.navbar-nav .nav-link').forEach(link => {
        link.classList.toggle('active', link.dataset.page === page);
    });
}

function showToast(message, type = 'success') {
    const container = document.getElementById('toast-container');
    if (!container) return;

    const toast = document.createElement('div');
    toast.className = `alert alert-${type} alert-dismissible fade show`;
    toast.innerHTML = `
        ${message}
        <button type="button" class="btn-close" data-bs-dismiss="alert"></button>
    `;
    container.appendChild(toast);
    setTimeout(() => toast.remove(), 4000);
}
