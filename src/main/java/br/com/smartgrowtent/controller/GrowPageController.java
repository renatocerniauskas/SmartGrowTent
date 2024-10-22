package br.com.smartgrowtent.controller;

import br.com.smartgrowtent.models.GrowValues;
import br.com.smartgrowtent.repositories.GrowValuesRepository;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;

@Controller
@Slf4j
public class GrowPageController {

    @Autowired
    GrowValuesRepository repository;

    @GetMapping("/grow")
    public String showGrow(Model model) {
        GrowValues lastValue = repository.findTopByOrderByTimestampDesc();
        if (lastValue != null) {
            double vpd = calculateVPD(lastValue.getTemperature(), lastValue.getHumidity());
            double idealHumidity = calculateIdealHumidity(lastValue.getTemperature(), vpd);
            model.addAttribute("valorVPD", Math.round(vpd * 100.0) / 100.0);
            model.addAttribute("idealHumidity", idealHumidity);
        } else {
            model.addAttribute("valorVPD", "N/A");
            model.addAttribute("idealHumidity", "N/A");
        }
        return "grow";
    }

    private double calculateVPD(double temperature, double humidity) {
        double Es = 0.6108 * Math.exp((17.27 * temperature) / (temperature + 237.3));
        double Ea = Es * (humidity / 100);
        return Es - Ea;
    }

    private double calculateIdealHumidity(double temperature, double idealVPD) {
        double Es = 0.6108 * Math.exp((17.27 * temperature) / (temperature + 237.3));
        return ((Es - idealVPD) / Es) * 100;
    }
}
