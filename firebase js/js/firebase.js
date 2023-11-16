// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getAnalytics } from "firebase/analytics";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyBPGox7HPoz_hMhCxWDVz1iq3NXYL7mmQM",
  authDomain: "gps-tracking-7ec2d.firebaseapp.com",
  databaseURL: "https://gps-tracking-7ec2d-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "gps-tracking-7ec2d",
  storageBucket: "gps-tracking-7ec2d.appspot.com",
  messagingSenderId: "550295610562",
  appId: "1:550295610562:web:09f43c0683bad1e58b475b",
  measurementId: "G-84PGVNCBBZ"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const analytics = getAnalytics(app);