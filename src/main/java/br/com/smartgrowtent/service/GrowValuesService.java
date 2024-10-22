package br.com.smartgrowtent.service;

import br.com.smartgrowtent.models.GrowValues;
import br.com.smartgrowtent.repositories.GrowValuesRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.LocalTime;
import java.util.List;

@Service
public class GrowValuesService {

    @Autowired
    private GrowValuesRepository repository;

    LocalDate now = LocalDate.now();

    public List<GrowValues> getGrowValues(final LocalDateTime start, final LocalDateTime end) {
        return repository.findByTimestampBetween(start, end);
    }

    public List<GrowValues> getGrowValues24last() {
        return getGrowValues(LocalDateTime.now().minusHours(24),  LocalDateTime.now());
    }
}
