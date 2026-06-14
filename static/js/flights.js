let allFlights = [];
let editModal;

async function loadFlights() {
    const search = document.getElementById('search-input').value.trim();
    const sort = document.getElementById('sort-select').value;

    const params = {};
    if (search) params.search = search;
    if (sort) params.sort = sort;

    try {
        allFlights = await fetchFlights(params);
        renderFlightsTable(allFlights);
    } catch (err) {
        showToast(err.message, 'danger');
    }
}

function renderFlightsTable(flights) {
    const tbody = document.getElementById('flights-body');
    tbody.innerHTML = '';

    if (flights.length === 0) {
        tbody.innerHTML = '<tr><td colspan="6" class="text-center text-muted py-5"><i class="bi bi-inbox d-block mb-2" style="font-size:2rem;opacity:0.3"></i>找不到航班</td></tr>';
        return;
    }

    flights.forEach(f => {
        const tr = document.createElement('tr');
        tr.innerHTML = `
            <td><span class="text-muted small">#${f.id}</span></td>
            <td><span class="flight-code">${f.flight_number}</span></td>
            <td><span class="route-text">${f.origin} <i class="bi bi-arrow-right mx-1"></i> ${f.destination}</span></td>
            <td><i class="bi bi-calendar3 me-1 text-muted"></i>${f.departure_date}</td>
            <td><span class="price-tag">${formatPrice(f.current_price)}</span></td>
            <td>
                <button class="btn btn-sm btn-outline-primary btn-action me-1" onclick="openEditModal(${f.id})" title="編輯">
                    <i class="bi bi-pencil"></i>
                </button>
                <button class="btn btn-sm btn-outline-danger btn-action" onclick="handleDelete(${f.id})" title="刪除">
                    <i class="bi bi-trash"></i>
                </button>
            </td>
        `;
        tbody.appendChild(tr);
    });
}

async function handleAdd(e) {
    e.preventDefault();
    const data = {
        flight_number: document.getElementById('add-number').value.trim(),
        origin: document.getElementById('add-origin').value.trim(),
        destination: document.getElementById('add-destination').value.trim(),
        departure_date: document.getElementById('add-date').value,
        current_price: parseFloat(document.getElementById('add-price').value)
    };

    try {
        await createFlight(data);
        showToast('航班新增成功');
        document.getElementById('add-form').reset();
        bootstrap.Modal.getInstance(document.getElementById('addModal')).hide();
        loadFlights();
    } catch (err) {
        showToast(err.message, 'danger');
    }
}

function openEditModal(id) {
    const flight = allFlights.find(f => f.id === id);
    if (!flight) return;

    document.getElementById('edit-id').value = flight.id;
    document.getElementById('edit-number').value = flight.flight_number;
    document.getElementById('edit-route').value = `${flight.origin} → ${flight.destination}`;
    document.getElementById('edit-date').value = flight.departure_date;
    document.getElementById('edit-price').value = flight.current_price;

    editModal.show();
}

async function handleEdit(e) {
    e.preventDefault();
    const id = document.getElementById('edit-id').value;
    const data = {
        departure_date: document.getElementById('edit-date').value,
        current_price: parseFloat(document.getElementById('edit-price').value)
    };

    try {
        await updateFlight(id, data);
        showToast('航班更新成功');
        editModal.hide();
        loadFlights();
    } catch (err) {
        showToast(err.message, 'danger');
    }
}

async function handleDelete(id) {
    if (!confirm('確定要刪除此航班？')) return;

    try {
        await deleteFlight(id);
        showToast('航班已刪除');
        loadFlights();
    } catch (err) {
        showToast(err.message, 'danger');
    }
}

document.addEventListener('DOMContentLoaded', () => {
    setActiveNav('flights');
    editModal = new bootstrap.Modal(document.getElementById('editModal'));

    document.getElementById('add-form').addEventListener('submit', handleAdd);
    document.getElementById('edit-form').addEventListener('submit', handleEdit);
    document.getElementById('search-btn').addEventListener('click', loadFlights);
    document.getElementById('sort-select').addEventListener('change', loadFlights);
    document.getElementById('search-input').addEventListener('keyup', e => {
        if (e.key === 'Enter') loadFlights();
    });

    loadFlights();
});
