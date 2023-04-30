package ru.mrgrd56.smarttonometer.dto;

class FailedMeasurementResultDto implements MeasurementResultDto {

    private final MeasurementResultStatus status;

    public FailedMeasurementResultDto(MeasurementResultStatus status) {
        this.status = status;
    }

    @Override
    public MeasurementResultStatus getStatus() {
        return this.status;
    }

    @Override
    public Integer getSys() {
        return null;
    }

    @Override
    public Integer getDia() {
        return null;
    }

    @Override
    public Integer getPulse() {
        return null;
    }

    @Override
    public Boolean getHasArrhythmia() {
        return null;
    }
}
