package ru.mrgrd56.smarttonometer.dto;

class OkMeasurementResultDto implements MeasurementResultDto {
    private final Integer sys;
    private final Integer dia;
    private final Integer pulse;
    private final Boolean hasArrhythmia;

    public OkMeasurementResultDto(Integer sys, Integer dia, Integer pulse, Boolean hasArrhythmia) {
        this.sys = sys;
        this.dia = dia;
        this.pulse = pulse;
        this.hasArrhythmia = hasArrhythmia;
    }

    @Override
    public MeasurementResultStatus getStatus() {
        return MeasurementResultStatus.OK;
    }

    @Override
    public Integer getSys() {
        return sys;
    }

    @Override
    public Integer getDia() {
        return dia;
    }

    @Override
    public Integer getPulse() {
        return pulse;
    }

    @Override
    public Boolean getHasArrhythmia() {
        return hasArrhythmia;
    }
}
