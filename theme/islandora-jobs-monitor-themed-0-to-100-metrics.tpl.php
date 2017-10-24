<div class="div_label"><?php print $label; ?></div>
<?php foreach ($values as $timestamp => $value) : ?>
  <div class="value-wrapper" title="<?php print $value . "% at " . $timestamp; ?>">
      <div class="value" title="<?php print $value . "% at " . $timestamp; ?>" style="height: <?php print 25-25*$value/100; ?>px">&nbsp;</div>
  </div>
<?php endforeach; ?>
<?php if (count($values) < 20) : ?>
  <?php for ($i = 1; $i <= (20 - count($values)); $i++) { ?>
  <div class="value-wrapper">
      <div class="value" style="height:25px">&nbsp;</div>
  </div>
  <?php } ?> 
<?php endif; ?>
<br class="clearfix">

