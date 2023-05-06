package ru.mrgrd56.smarttonometer;

import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.builder.SpringApplicationBuilder;

@SpringBootApplication
public class SmartTonometerApplication {

    public static void main(String[] args) {
        new SpringApplicationBuilder(SmartTonometerApplication.class)
                .headless(false)
                .run(args);
    }

}
