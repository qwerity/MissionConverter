let map
let markers = L.featureGroup()

function getElevation(latlng)
{
    let response = $.ajax({
        url: 'https://secure.geonames.org/srtm3JSON?'
            + '&lat=' + latlng.lat
            + '&lng=' + latlng.lng
            + '&username=galwayireland',
        dataType: 'json',
        async: false
    })

    return L.latLng(response.responseJSON.lat, response.responseJSON.lng, response.responseJSON.srtm3)
}

function prepareContent({lat, lng, alt})
{
    let content = `(${lat.toFixed(7)}, ${lng.toFixed(7)})<br>`
    content += `<strong>Latitude</strong>: ${lat}<br>`
    content += `<strong>Longitude</strong>: ${lng}<br>`
    content += `<strong>Altitude</strong>: ${alt} m<br>`
    return content
}

function removeAllMarkers()
{
    markers.clearLayers()
}

function addMarker(latlng)
{
    const latlngalt = getElevation(latlng)
    const marker = new L.marker(latlngalt, {draggable: true})

    let markerPopup = L.popup()
    markers.addLayer(marker)
    markerPopup.setContent(prepareContent(latlngalt))
    marker.bindPopup(markerPopup).openPopup()

    marker.on('contextmenu', (e) =>
    {
        let markerContextMenuPopup = L.popup()

        let btn = L.DomUtil.create('button')
        btn.innerHTML = 'Remove'
        btn.onclick = function ()
        {
            map.removeLayer(marker)
            map.removeLayer(markerContextMenuPopup)
        }
        markerContextMenuPopup.setLatLng(e.latlng).setContent(btn).addTo(map).openOn(map)
    })
    marker.on('drag', function (event)
    {
        let latlng = event.target.getLatLng()
        markerPopup.setContent(prepareContent(latlng))
        marker.openPopup()
    })
    marker.on('dragend', function (event)
    {
        const latlngalt = getElevation(event.target.getLatLng())
        markerPopup.setContent(prepareContent(latlngalt))
        marker.openPopup()
    })

    return latlngalt
}

function onMapClick(event)
{
    const latlngalt = addMarker(event.latlng)

    MainWindow.onNewPoint(latlngalt.lat, latlngalt.lng, latlngalt.alt)
    map.flyTo(latlngalt);  // you can specify pan/zoom options as well
}

function onMapMove(event)
{
    const lat = event.latlng.lat
    const lng = event.latlng.lng
    MainWindow.onMapMove(lat, lng)
}

function addLocationButton(map)
{
    let lc = L.control.locate({
        locateOptions: { enableHighAccuracy: true },
        keepCurrentZoomLevel: true,
        flyTo: true,
        cacheLocation: false
    }).addTo(map)
    //lc.start()
}
function addPolylineMeasure(map)
{
    L.control.polylineMeasure({
        position: 'topleft',
        unit: 'metres',
        showBearings: false,
        clearMeasurementsOnStop: false,
        showClearControl: true,
        showUnitControl: true
    }).addTo(map)

    map.on('polylinemeasure:new-point', ({index, latlngalt}) => {
        MainWindow.onNewPoint(index, latlngalt.lat, latlngalt.lng, latlngalt.alt)
    });
    map.on('polylinemeasure:move', e => {
        const latlngalt = getElevation(e.latlng)
        MainWindow.onNewPoint(e.sourceTarget.cntCircle, latlngalt.lat, latlngalt.lng, latlngalt.alt)
    });
}

function initQWebChannel()
{
    new QWebChannel(qt.webChannelTransport, function (channel) {
        window.MainWindow = channel.objects.MainWindow
        if('undefined' !== typeof MainWindow)
        {
            MainWindow.addPoint.connect((lat, lng) => {
                const latlng = {lat, lng}
                addMarker(latlng)
                map.flyTo(latlng);
            })
            MainWindow.removeAllPoints.connect(removeAllMarkers)
        }
    })
}

function initMap()
{
    const osm = L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {maxZoom: 18})
    const ArcGIS_satellite = L.tileLayer('http://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/{z}/{y}/{x}', {maxZoom: 18})

    const esri_hybrid = L.esri.basemapLayer('ImageryLabels')
    const esri_satellite = L.esri.basemapLayer('Imagery')
    const esri_terrain = L.esri.basemapLayer('Topographic')

    const gmaps_hybrid = L.tileLayer('http://{s}.google.com/vt/lyrs=s,h&x={x}&y={y}&z={z}', {maxZoom: 22, subdomains:['mt0','mt1','mt2','mt3']})
    const gmaps_satellite = L.tileLayer('http://{s}.google.com/vt/lyrs=s&x={x}&y={y}&z={z}',{maxZoom: 22, subdomains:['mt0','mt1','mt2','mt3']})
    const gmaps_terrain = L.tileLayer('http://{s}.google.com/vt/lyrs=p&x={x}&y={y}&z={z}',  {maxZoom: 22, subdomains:['mt0','mt1','mt2','mt3']})

    let sHybrid = L.layerGroup([esri_satellite, esri_hybrid])
    map = L.map('map', { layers: sHybrid }).setView([40.295743, 44.566257], 12)

    const baseLayers =
    {
        "OSM": osm,
        "Esri Satellite": esri_satellite,
        "ArcGIS Satellite": ArcGIS_satellite,
        "GMap Satellite": gmaps_satellite,
        "Esri Hybrid":  sHybrid,
        "GMap Hybrid": gmaps_hybrid,
        "Esri Terrain": esri_terrain,
        "GMap Terrain": gmaps_terrain,
    }
    L.control.layers(baseLayers, {}, {position: 'topright', collapsed: true}).addTo(map)

    markers.addTo(map);

    addPolylineMeasure(map)
    // addLocationButton(map)

    map.on('mousemove', onMapMove)
    // map.on('click', onMapClick)
}

function initialize()
{
    initQWebChannel()
    initMap()
}
