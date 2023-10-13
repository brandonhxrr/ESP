package com.brandonhxrr.esp

import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.google.android.material.textfield.TextInputEditText
import org.json.JSONObject
import java.io.BufferedReader
import java.io.InputStreamReader
import java.net.HttpURLConnection
import java.net.URL
import java.util.Timer
import java.util.TimerTask

class MainActivity : AppCompatActivity() {

    lateinit var tvRPM: TextView
    lateinit var tvTemp: TextView
    lateinit var tvCapacitance: TextView
    lateinit var tvVoltage: TextView
    lateinit var txtIP: TextInputEditText
    lateinit var btnConnect: Button

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        tvRPM = findViewById(R.id.tvRPM)
        tvTemp = findViewById(R.id.tvTemperature)
        tvCapacitance = findViewById(R.id.tvCapacitance)
        tvVoltage = findViewById(R.id.tvVoltage)
        txtIP = findViewById(R.id.txt_ip)
        btnConnect = findViewById(R.id.btn_connect)

        btnConnect.setOnClickListener {
            val ipAddress = txtIP.text.toString()
            if (isIPAddress(ipAddress)) {
                val timer = Timer()

                timer.scheduleAtFixedRate(object : TimerTask() {
                    override fun run() {
                        fetchDataFromESP32(ipAddress)
                    }
                }, 0, 1500)
            } else {
                Toast.makeText(
                    applicationContext,
                    "No es una dirección IP válida",
                    Toast.LENGTH_SHORT
                ).show()
            }

        }
    }

    private fun fetchDataFromESP32(ip: String) {
        try {
            //val url = URL("http://$ip:8080/")
            val url = URL("https://9d95-201-141-109-189.ngrok.io/")
            val connection = url.openConnection() as HttpURLConnection
            connection.requestMethod = "GET"

            val reader = BufferedReader(InputStreamReader(connection.inputStream))
            val responseStringBuilder = StringBuilder()
            var line: String?

            while (reader.readLine().also { line = it } != null) {
                responseStringBuilder.append(line)
            }

            val response = responseStringBuilder.toString()

            if (response.isNotEmpty()) {
                val json = JSONObject(response)
                val rpm = json.getDouble("rpm")
                val capacitance = json.getDouble("capacitancia")
                val temperature = json.getDouble("temperatura")
                val voltage = json.getDouble("voltaje")

                val capacitanceUnits = if (capacitance < 1E-6) "pF" else "nF"

                Handler(Looper.getMainLooper()).post {
                    tvRPM.text = String.format("%.2f rpm", rpm)
                    tvTemp.text = String.format("%.2f °C", temperature)
                    tvCapacitance.text = String.format("%.2f %s", capacitance, capacitanceUnits)
                    tvVoltage.text = String.format("%.2f V", voltage)
                }

            }
        } catch (e: Exception) {
            e.printStackTrace()
            Log.e("MainActivity", "Error: ${e.message}")
        }
    }
}
