
// Import the functions you need from the SDKs you need
import { initializeApp } from "https://www.gstatic.com/firebasejs/10.6.0/firebase-app.js";
import { getAnalytics } from "https://www.gstatic.com/firebasejs/10.6.0/firebase-analytics.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/10.6.0/firebase-database.js";

// Your web app's Firebase configuration
const firebaseConfig = {
    apiKey: "AIzaSyBPGox7HPoz_hMhCxWDVz1iq3NXYL7mmQM",
    authDomain: "gps-tracking-7ec2d.firebaseapp.com",
    databaseURL: "https://gps-tracking-7ec2d-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "gps-tracking-7ec2d",
    storageBucket: "gps-tracking-7ec2d.appspot.com",
    messagingSenderId: "550295610562",
    appId: "1:550295610562:web:31d1331495a282b38b475b",
    measurementId: "G-F50PGNBNGM"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const analytics = getAnalytics(app);
const database = getDatabase(app);

// Your existing code for reading data from Firebase
const dataRef1 = ref(database, 'DHT_11/Humidity');
const dataRef2 = ref(database, 'DHT_11/Temperature');
const dateTimeRef = ref(database, 'DateTime');
const locationRef = ref(database, 'Location');
const doorRef = ref(database, 'DoorStatus');
const speedRef = ref(database, 'Speed');
onValue(dataRef1, (snapshot) => {
    const humi = snapshot.val();
    document.getElementById('humidity').innerHTML = humi + "%";
});

onValue(dataRef2, (snapshot) => {
    const temp = snapshot.val();
    document.getElementById('temperature').innerHTML = temp + "&#8451;";
});
onValue(doorRef, (snapshot) => {
    const door = snapshot.val();
    document.getElementById('doorStatus').innerHTML = (!door) ? " Đóng" : " Mở";
});
onValue(speedRef, (snapshot) => {
    const speed = snapshot.val();
    document.getElementById('speed').innerHTML = speed + "Km/h";
});
onValue(dateTimeRef, (snapshot) => {
    const dateTime = snapshot.val();
    document.getElementById('date').innerHTML = "Ngày: " + dateTime.Date + " / " + dateTime.Month + " / " + dateTime.Year;
    document.getElementById('time').innerHTML = "Giờ: " + dateTime.Hour + " : " + dateTime.Minute + " : " + dateTime.Second +"s";
});

onValue(locationRef, (snapshot) => {
    const location = snapshot.val();
    document.getElementById('latitude').innerHTML = "Latitude: " + location.Latitude;
    document.getElementById('longitude').innerHTML = "Longitude: " + location.Longitude;

    // Update Google Maps link
    const mapLinkElement = document.getElementById('map-link');


    const mapLink = `https://www.google.com/maps?q=${location.Latitude},${location.Longitude}`;
    mapLinkElement.href = mapLink;
    mapLinkElement.innerHTML = "Xem vị trí";

});

