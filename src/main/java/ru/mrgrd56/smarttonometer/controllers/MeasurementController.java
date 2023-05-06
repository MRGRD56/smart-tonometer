package ru.mrgrd56.smarttonometer.controllers;

import org.springframework.web.bind.annotation.*;
import ru.mrgrd56.smarttonometer.dto.MeasurementHand;
import ru.mrgrd56.smarttonometer.dto.MeasurementResultDto;

import java.awt.*;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.time.Instant;
import java.util.Random;
import java.util.UUID;

@RestController
@RequestMapping("measurement")
public class MeasurementController {
    @PostMapping
    public MeasurementResultDto addMeasurement(
            @RequestParam MeasurementHand hand,
            @RequestBody(required = false) byte[] photo) {
        try {
            Path path = Files.write(Path.of("C:\\_op\\esphoto\\esphoto_" + Instant.now().getEpochSecond() + ".jpg"), photo);
            Desktop.getDesktop().open(path.toFile());
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        var random = new Random();

        return MeasurementResultDto.ok(
                random.nextInt(100, 140 + 1),
                random.nextInt(60, 80 + 1),
                random.nextInt(60, 90),
                random.nextBoolean());
    }
}
