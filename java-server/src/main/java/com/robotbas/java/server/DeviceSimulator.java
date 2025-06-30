/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package com.robotbas.java.server;

import java.util.Random;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author berna
 */
public class DeviceSimulator extends Thread {
    private Random r = new Random();
    private DeviceSimulatorListener simulationListener;
    private String deviceName;
    
    public DeviceSimulator(String deviceName) {
        this.simulationListener = null;
        this.deviceName = deviceName;
    }
    
    public void subscribe(DeviceSimulatorListener simulationListener) {
        this.simulationListener = simulationListener;
    }

    @Override
    public void run() {
        while(true) {
            DeviceData data = this.randomDeviceData();
            if (simulationListener != null)
                simulationListener.onDeviceDataChange(data);
            try {
                Thread.sleep(r.nextInt(0, 5));
            } catch (InterruptedException ex) {
                Logger.getLogger(DeviceSimulator.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    
    private DeviceData randomDeviceData() {
        float minTemperature = 20;
        float maxTemperature = 40f;
        DeviceData data = new DeviceData();
        data.deviceName = this.deviceName;
        data.temperature = minTemperature + r.nextFloat() * (maxTemperature - minTemperature);
        return data;
    }
}
