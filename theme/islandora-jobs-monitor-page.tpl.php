<div class="masters">
  <label>Master/s</label>
  <?php foreach ($themed_hosts['master'] as $themed_host): ?>
    <?php echo $themed_host;?>
  <?php endforeach; ?>
</div>

<div class="workers">
  <label>Worker/s</label>
  <?php foreach ($themed_hosts['workers'] as $themed_host): ?>
    <?php echo $themed_host;?>
  <?php endforeach; ?>
</div>

<div id="tabs" class="tab-report">
  <ul>
    <li><a href="#tabs-1">Jobs Records</a></li>
    <li><a href="#tabs-2">Gearman Queue</a></li>
  </ul>
  <div id="tabs-1" class="monitor_report"><?php print $current_report; ?></div>
  <div id="tabs-2" class="gearman_queue"><?php print $gearman_queue; ?></div>
</div>