package ru.mrgrd56.smarttonometer.dto;

public interface MeasurementResultDto {
    static MeasurementResultDto failed(MeasurementResultStatus status) {
        return new FailedMeasurementResultDto(status);
    }

    static MeasurementResultDto ok(int sys, int dia, int pulse, boolean hasArrhythmia) {
        return new OkMeasurementResultDto(sys, dia, pulse, hasArrhythmia);
    }

    MeasurementResultStatus getStatus();

    Integer getSys();

    Integer getDia();

    Integer getPulse();

    Boolean getHasArrhythmia();
}
