package br.com.smartgrowtent.service;

import br.com.smartgrowtent.models.GrowValues;
import br.com.smartgrowtent.repositories.GrowValuesRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.time.LocalDate;
import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;

@Service
public class GrowValuesService {

    @Autowired
    private GrowValuesRepository repository;

    LocalDate now = LocalDate.now();

    public List<GrowValues> getGrowValues(final LocalDateTime start, final LocalDateTime end) {
        return repository.findByTimestampBetween(start, end);
    }

    public List<GrowValues> getGrowValuesLastHours(final int hours) {
        return getGrowValues(LocalDateTime.now().minusHours(hours),  LocalDateTime.now());
    }

    public Optional<GrowValues> getGrowValuesLast() {
        return Optional.ofNullable(repository.findTopByOrderByTimestampDesc());
    }
}
