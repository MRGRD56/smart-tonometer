package ru.mrgrd56.smarttonometer.controllers;

import org.springframework.web.bind.annotation.*;
import ru.mrgrd56.smarttonometer.dto.MeasurementHand;
import ru.mrgrd56.smarttonometer.dto.MeasurementResultDto;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.time.Instant;
import java.util.UUID;

@RestController
@RequestMapping("measurement")
public class MeasurementController {
    @PostMapping
    public MeasurementResultDto addMeasurement(
            @RequestParam MeasurementHand hand,
            @RequestBody(required = false) byte[] photo) {
        try {
            Files.write(Path.of("C:\\_op\\esphoto_" + Instant.now().getEpochSecond() + ".jpg"), photo);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        return MeasurementResultDto.ok(124, 71, 81, false);
    }
}
